import psycopg2
import pandas as pd

def query_telemetry():
    conn = psycopg2.connect(
        dbname='auss',
        user='postgres',
        password='lol_lmao',
        host='localhost'
    )
    
    query = "SELECT * FROM telemetry ORDER BY created_at DESC"
    df = pd.read_sql(query, conn)
    
    print("All telemetry data:")
    print(df.to_string())
    
    print("\n\nStatistics by workload type:")
    query = """
        SELECT 
            workload_type,
            COUNT(*) as num_processes,
            AVG(cpu_percent) as avg_cpu,
            MAX(cpu_percent) as max_cpu,
            MIN(cpu_percent) as min_cpu,
            AVG(io_throughput_mbps) as avg_io
        FROM telemetry
        GROUP BY workload_type
    """
    stats = pd.read_sql(query, conn)
    print(stats.to_string())
    
    conn.close()

if __name__ == '__main__':
    query_telemetry()