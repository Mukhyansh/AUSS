/*
 * AUSS Telemetry Collector (for Workloads)
 * 
 * Reads PIDs from pid_queue.txt and collects telemetry data.
 * 
 * Architecture:
 *   1. Reads PIDs from pid_queue.txt (written by Activate_Workloads)
 *   2. For each PID, samples /proc/[pid]/stat and /proc/[pid]/io
 *   3. Aggregates metrics (CPU%, I/O throughput, etc.)
 *   4. Stores results to CSV file
 * 
 * Compile:
 *   gcc -Wall -O2 -o telemetry_collector telemetry_collector.c
 * 
 * Run:
 *   ./telemetry_collector
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>

/* ========================================================================== */
/* CONFIGURATION */
/* ========================================================================== */

#define PID_QUEUE_FILE "pid_queue.txt"
#define OUTPUT_CSV_FILE "telemetry_results.csv"
#define SAMPLE_INTERVAL_MS 100  /* Sample every 100ms */
#define MAX_PIDS 2000
#define MAX_SAMPLES 5000

/* ========================================================================== */
/* DATA STRUCTURES */
/* ========================================================================== */

struct proc_stat {
    long utime;
    long stime;
    int nice;
};

struct proc_io {
    long read_bytes;
    long write_bytes;
    long read_ops;
    long write_ops;
};

struct sample {
    struct proc_stat stat;
    struct proc_io io;
};

struct workload {
    char name[256];
    char type[32];
    pid_t pid;
    time_t start_time;
    struct sample *samples;
    int num_samples;
};

struct result {
    char name[256];
    char type[32];
    pid_t pid;
    long elapsed_ms;
    double cpu_percent;
    double io_throughput_mbps;
    double io_ops_per_sec;
    int samples_collected;
};

/* ========================================================================== */
/* LOGGING */
/* ========================================================================== */

void log_info(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now);
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%H:%M:%S", timeinfo);
    
    printf("[%s] [INFO] ", timestamp);
    vprintf(fmt, args);
    printf("\n");
    
    va_end(args);
}

void log_error(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now);
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%H:%M:%S", timeinfo);
    
    fprintf(stderr, "[%s] [ERROR] ", timestamp);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    
    va_end(args);
}

#include <stdarg.h>

/* ========================================================================== */
/* READ /PROC METRICS */
/* ========================================================================== */

int read_proc_stat(pid_t pid, struct proc_stat *stat) {
    char path[256];
    FILE *fp;
    char buffer[4096];
    
    snprintf(path, sizeof(path), "/proc/%d/stat", pid);
    
    fp = fopen(path, "r");
    if (!fp) {
        return 0;
    }
    
    if (!fgets(buffer, sizeof(buffer), fp)) {
        fclose(fp);
        return 0;
    }
    fclose(fp);
    
    /* Parse: pid (comm) state ppid ... utime stime ... */
    if (sscanf(buffer, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %ld %ld %*d %*d %*d %d",
               &stat->utime, &stat->stime, &stat->nice) != 3) {
        return 0;
    }
    
    return 1;
}

int read_proc_io(pid_t pid, struct proc_io *io) {
    char path[256];
    FILE *fp;
    char line[256];
    
    snprintf(path, sizeof(path), "/proc/%d/io", pid);
    
    fp = fopen(path, "r");
    if (!fp) {
        return 0;
    }
    
    io->read_bytes = 0;
    io->write_bytes = 0;
    io->read_ops = 0;
    io->write_ops = 0;
    
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "read_bytes:", 11) == 0) {
            sscanf(line, "read_bytes: %ld", &io->read_bytes);
        }
        else if (strncmp(line, "write_bytes:", 12) == 0) {
            sscanf(line, "write_bytes: %ld", &io->write_bytes);
        }
        else if (strncmp(line, "read_ops:", 9) == 0) {
            sscanf(line, "read_ops: %ld", &io->read_ops);
        }
        else if (strncmp(line, "write_ops:", 10) == 0) {
            sscanf(line, "write_ops: %ld", &io->write_ops);
        }
    }
    
    fclose(fp);
    return 1;
}

int process_is_alive(pid_t pid) {
    return kill(pid, 0) == 0;
}

/* ========================================================================== */
/* COLLECT TELEMETRY */
/* ========================================================================== */

void collect_telemetry_for_pid(struct workload *winfo) {
    log_info("Collecting telemetry for PID %d (%s)", winfo->pid, winfo->name);
    
    time_t start_time = time(NULL);
    int max_duration = 120;  /* Max 120 seconds */
    
    /* Allocate sample buffer */
    winfo->samples = malloc(sizeof(struct sample) * MAX_SAMPLES);
    if (!winfo->samples) {
        log_error("Memory allocation failed");
        return;
    }
    winfo->num_samples = 0;
    
    /* Sample while process is alive */
    while (process_is_alive(winfo->pid)) {
        /* Check timeout */
        if (time(NULL) - start_time > max_duration) {
            log_error("Timeout collecting telemetry for PID %d", winfo->pid);
            break;
        }
        
        /* Check buffer space */
        if (winfo->num_samples >= MAX_SAMPLES) {
            log_error("Sample buffer full for PID %d", winfo->pid);
            break;
        }
        
        /* Read metrics */
        struct sample *s = &winfo->samples[winfo->num_samples];
        
        if (read_proc_stat(winfo->pid, &s->stat) &&
            read_proc_io(winfo->pid, &s->io)) {
            winfo->num_samples++;
        }
        
        /* Sleep before next sample */
        usleep(SAMPLE_INTERVAL_MS * 1000);
    }
    
    if (winfo->num_samples == 0) {
        log_error("No samples collected for PID %d", winfo->pid);
        free(winfo->samples);
        winfo->samples = NULL;
        return;
    }
    
    log_info("Collected %d samples for PID %d", winfo->num_samples, winfo->pid);
}

/* ========================================================================== */
/* AGGREGATE METRICS */
/* ========================================================================== */

void aggregate_metrics(struct workload *winfo, struct result *result) {
    if (!winfo->samples || winfo->num_samples < 2) {
        log_error("Cannot aggregate - not enough samples");
        return;
    }
    
    struct sample *first = &winfo->samples[0];
    struct sample *last = &winfo->samples[winfo->num_samples - 1];
    
    /* Calculate elapsed time */
    long elapsed_ms = (long)(winfo->num_samples - 1) * SAMPLE_INTERVAL_MS;
    if (elapsed_ms <= 0) elapsed_ms = 1;
    
    /* Calculate CPU time difference (in jiffies) */
    long cpu_time_jiffies = (last->stat.utime - first->stat.utime) +
                            (last->stat.stime - first->stat.stime);
    
    /* Convert jiffies to ms (assuming 100 Hz = 10ms per jiffy) */
    #define JIFFY_TO_MS 10
    long cpu_time_ms = cpu_time_jiffies * JIFFY_TO_MS;
    
    /* CPU percentage */
    double cpu_percent = (double)cpu_time_ms / elapsed_ms * 100.0;
    
    /* Calculate I/O throughput */
    long total_read = last->io.read_bytes - first->io.read_bytes;
    long total_write = last->io.write_bytes - first->io.write_bytes;
    long total_io_bytes = total_read + total_write;
    
    double io_throughput_mbps = (double)total_io_bytes / elapsed_ms / 1e6;
    
    /* I/O ops per second */
    long io_ops = (last->io.read_ops - first->io.read_ops) +
                  (last->io.write_ops - first->io.write_ops);
    double io_ops_per_sec = (double)io_ops / (elapsed_ms / 1000.0);
    
    /* Fill result */
    strncpy(result->name, winfo->name, sizeof(result->name) - 1);
    strncpy(result->type, winfo->type, sizeof(result->type) - 1);
    result->pid = winfo->pid;
    result->elapsed_ms = elapsed_ms;
    result->cpu_percent = cpu_percent;
    result->io_throughput_mbps = io_throughput_mbps;
    result->io_ops_per_sec = io_ops_per_sec;
    result->samples_collected = winfo->num_samples;
}

/* ========================================================================== */
/* READ QUEUE AND PROCESS */
/* ========================================================================== */

int read_and_process_queue(struct result *results, int *num_results) {
    FILE *fp;
    char line[512];
    
    log_info("Waiting for queue file: %s", PID_QUEUE_FILE);
    
    /* Wait for queue file to appear (with timeout) */
    int wait_count = 0;
    while (access(PID_QUEUE_FILE, F_OK) != 0 && wait_count < 60) {
        log_info("Waiting for queue file... (%d/60)", wait_count + 1);
        sleep(1);
        wait_count++;
    }
    
    if (access(PID_QUEUE_FILE, F_OK) != 0) {
        log_error("Queue file not found: %s", PID_QUEUE_FILE);
        log_error("Make sure Activate_Workloads is running first!");
        return 0;
    }
    
    log_info("Queue file found!");
    
    /* Give processes time to start */
    log_info("Waiting for processes to start...");
    sleep(2);
    
    /* Read queue */
    fp = fopen(PID_QUEUE_FILE, "r");
    if (!fp) {
        log_error("Failed to open queue file");
        return 0;
    }
    
    int queue_size = 0;
    struct workload workloads[MAX_PIDS];
    
    while (fgets(line, sizeof(line), fp)) {
        /* Parse: "workload_name,workload_type,pid,start_time" */
        char name[256];
        char type[32];
        pid_t pid;
        long start_time;
        
        if (sscanf(line, "%255[^,],%31[^,],%d,%ld",
                   name, type, &pid, &start_time) != 4) {
            continue;
        }
        
        if (queue_size >= MAX_PIDS) {
            log_error("Queue too large, stopping");
            break;
        }
        
        strncpy(workloads[queue_size].name, name, sizeof(workloads[queue_size].name) - 1);
        strncpy(workloads[queue_size].type, type, sizeof(workloads[queue_size].type) - 1);
        workloads[queue_size].pid = pid;
        workloads[queue_size].start_time = start_time;
        workloads[queue_size].samples = NULL;
        workloads[queue_size].num_samples = 0;
        
        log_info("Queued PID %d (%s)", pid, name);
        queue_size++;
    }
    
    fclose(fp);
    
    if (queue_size == 0) {
        log_error("Queue is empty!");
        return 0;
    }
    
    log_info("Found %d PIDs in queue", queue_size);
    
    /* Collect telemetry for each PID */
    *num_results = 0;
    for (int i = 0; i < queue_size; i++) {
        collect_telemetry_for_pid(&workloads[i]);
        
        if (workloads[i].samples) {
            aggregate_metrics(&workloads[i], &results[*num_results]);
            
            log_info("✓ %s: CPU %.2f%%, I/O %.4f MB/s",
                     workloads[i].name,
                     results[*num_results].cpu_percent,
                     results[*num_results].io_throughput_mbps);
            
            (*num_results)++;
            
            free(workloads[i].samples);
        } else {
            log_error("✗ Failed to collect telemetry for PID %d", workloads[i].pid);
        }
    }
    
    return 1;
}

/* ========================================================================== */
/* STORE RESULTS TO CSV */
/* ========================================================================== */

void store_results_to_csv(struct result *results, int num_results) {
    FILE *fp;
    
    if (num_results == 0) {
        log_error("No results to store");
        return;
    }
    
    fp = fopen(OUTPUT_CSV_FILE, "w");
    if (!fp) {
        log_error("Failed to open output file: %s", OUTPUT_CSV_FILE);
        return;
    }
    
    /* Header */
    fprintf(fp, "workload_name,workload_type,pid,elapsed_ms,cpu_percent,io_throughput_mbps,io_ops_per_sec,samples_collected\n");
    
    /* Data */
    for (int i = 0; i < num_results; i++) {
        fprintf(fp, "%s,%s,%d,%ld,%.2f,%.4f,%.2f,%d\n",
                results[i].name,
                results[i].type,
                results[i].pid,
                results[i].elapsed_ms,
                results[i].cpu_percent,
                results[i].io_throughput_mbps,
                results[i].io_ops_per_sec,
                results[i].samples_collected);
    }
    
    fclose(fp);
    log_info("Stored %d results to %s", num_results, OUTPUT_CSV_FILE);
}

/* ========================================================================== */
/* MAIN */
/* ========================================================================== */

int main(void) {
    struct result results[MAX_PIDS];
    int num_results = 0;
    
    log_info("========================================");
    log_info("AUSS Telemetry Collector Starting");
    log_info("========================================");
    log_info("Reading PIDs from: %s", PID_QUEUE_FILE);
    log_info("Output CSV: %s", OUTPUT_CSV_FILE);
    log_info("");
    
    /* Read queue and collect telemetry */
    if (!read_and_process_queue(results, &num_results)) {
        log_error("Failed to process queue");
        return 1;
    }
    
    /* Store results */
    store_results_to_csv(results, num_results);
    
    /* Summary */
    log_info("");
    log_info("========================================");
    log_info("Telemetry Collection Finished");
    log_info("Results: %d collected", num_results);
    log_info("Output: %s", OUTPUT_CSV_FILE);
    log_info("========================================");
    
    return 0;
}