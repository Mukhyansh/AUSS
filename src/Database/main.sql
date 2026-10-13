CREATE DATABASE auss;
\c auss

CREATE TABLE telemetry(
    id SERIAL PRIMARY KEY,
    workload_name VARCHAR(50),
    workload_type VARCHAR(50),
    pid INT,
    io_throughput_mbps FLOAT,
    cpu_percent FLOAT,
    io_ops_per_sec FLOAT,
    elapsed_ms BIGINT,
    num_samples INT,
    created_at TIMESTAMP DEFAULT NOW()
);

CREATE INDEX idx_pid ON telemetry(pid);
CREATE INDEX idx_type ON telemetry(workload_type);