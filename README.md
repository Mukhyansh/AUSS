# AUSS - Adaptive User-space Scheduler for Linux OS

An end-to-end system for collecting process telemetry, analyzing workload characteristics, and training machine learning models to optimize Linux process scheduling policies.

## Project Overview

AUSS is a closed-loop system that:
1. **Generates** synthetic workloads (CPU-heavy, I/O-heavy, mixed)
2. **Collects** telemetry data from `/proc/[pid]/` metrics
3. **Stores** data in PostgreSQL database
4. **Trains** RandomForest classifier to categorize process types
5. **Applies** optimized scheduling policies (nice, setpriority, sched_setaffinity)
6. **Measures** performance improvements vs baseline

## Architecture

```
┌─────────────────────────────────────────────────────────┐
│                    AUSS Pipeline                        │
├─────────────────────────────────────────────────────────┤
│                                                         │
│  Phase 1: Workload Generation                           │
│  ├─ Activate_Workloads (launches synthetic tasks)       │
│  ├─ byteReader (I/O-heavy)                              │
│  ├─ matrixMultiplication (CPU-heavy)                    │
│  └─ compressor (mixed: CPU + I/O)                       │
│                                                         │
│  Phase 2: Telemetry Collection                          │
│  ├─ Samples /proc/[pid]/stat (CPU metrics)              │
│  ├─ Samples /proc/[pid]/io (I/O metrics)                │
│  ├─ Collects every 100ms for ~25 seconds per process    │
│  └─ Outputs: result.csv                                 │
│                                                         │
│  Phase 3: Data Storage                                  │
│  ├─ Loads CSV into PostgreSQL                           │
│  ├─ Creates schema with indexes                         │
│  └─ Enables SQL queries on telemetry                    │
│                                                         │
│  Phase 4: ML Training                                   │
│  ├─ Queries telemetry from database                     │
│  ├─ Trains RandomForest classifier                      │
│  ├─ Predicts workload type from metrics                 │
│  └─ Saves model: scheduler_model.pkl                    │
│                                                         │
│  Phase 5: Policy Application (Future)                   │
│  └─ Use predictions to apply optimized policies         │
│  	(Rest in dev!)                                        │
└─────────────────────────────────────────────────────────┘
```

## Directory Structure *(till now)*

```
AUSS/
├── include/
├── metrics/
├── src/
│   ├── Database/
│   │   ├── main.py          (CSV to PostgreSQL loader)
│   │   ├── main.sql         (Database schema)
│   │   └── test.py          (Query and analysis)
│   ├── Junk/
│   │   └── input_file.bin   (I/O test data)
│   ├── Results/
│   ├── Scheduler/
│   ├── Telemetry/
│   │   ├── collector.c      (Telemetry collection daemon)
│   │   ├── collector        (compiled binary)
│   │   └── result.csv       (collected metrics)
│   └── Workloads/
│       ├── Activate_Workloads.c    (workload launcher)
│       ├── Activate_Workloads      (compiled binary)
│       ├── byteReader.c            (I/O-heavy workload)
│       ├── matrixMultiplication.c  (CPU-heavy workload)
│       ├── comp.c                  (compressor driver)
│       ├── Compressor/             (compression library)
│       └── Makefile
├── .gitignore
└── README.md
```

## Prerequisites

### System Requirements
- Linux OS (Ubuntu, Arch, Fedora, etc)
- GCC compiler
- PostgreSQL server
- Python 3.8+

### Dependencies

**C/OS Level:**
```bash
# Debian/Ubuntu
sudo apt install build-essential postgresql postgresql-contrib

# Arch Linux
sudo pacman -S base-devel postgresql
```

**Python Level:**
```bash
pip install pandas psycopg2-binary scikit-learn
```


## References

- [Linux /proc filesystem](https://www.baeldung.com/linux/total-process-cpu-usage)
- [PostgreSQL Documentation](https://www.geeksforgeeks.org/postgresql/postgresql-connecting-to-the-database-using-python/)
- [scikit-learn RandomForest](https://scikit-learn.org/stable/model_selection.html)

## Author

Mukhyansh
