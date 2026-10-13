import pandas as pd
import psycopg2
from psycopg2.extras import execute_values

def load_csv_to_postgresql(csv_file, db_name='auss', user='postgres', password='your_password'):
    try:
        conn = psycopg2.connect(
            dbname=db_name,
            user=user,
            password=password,
            host='localhost'
        )
        cur = conn.cursor()
        
        df = pd.read_csv(csv_file)
        df.columns = df.columns.str.strip()
        
        print(f"Loaded {len(df)} rows from {csv_file}")
        
        data = []
        for _, row in df.iterrows():
            data.append((
                row['workload_name'],
                row['workload_type'],
                int(row['pid']),
                int(row['elapsed_ms']),
                float(row['cpu_percent']),
                float(row['io_throughput_mbps']),
                float(row['io_ops_per_sec']),
                int(row['samples_collected'])
            ))
        
        query = """
            INSERT INTO telemetry 
            (workload_name, workload_type, pid, elapsed_ms, cpu_percent, 
             io_throughput_mbps, io_ops_per_sec, num_samples)
            VALUES (%s, %s, %s, %s, %s, %s, %s, %s)
        """
        
        cur.executemany(query, data)
        conn.commit()
        
        print(f"Successfully inserted {len(data)} rows")
        
        cur.execute("SELECT COUNT(*) FROM telemetry")
        total = cur.fetchone()[0]
        print(f"Total rows in database: {total}")
        
        cur.close()
        conn.close()
        
    except Exception as e:
        print(f"Error: {e}")

if __name__ == '__main__':
    load_csv_to_postgresql('../Telemetry/result.csv')