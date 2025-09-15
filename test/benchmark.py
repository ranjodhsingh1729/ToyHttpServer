import requests
import concurrent.futures
import time

# Configuration
URL = "http://127.0.0.1:8000"
NUM_REQUESTS = 10000
MAX_WORKERS = 10000

def send_request(session):
    """
    Sends one GET request using the given session.
    Returns (status_code_or_None, elapsed_seconds).
    """
    start = time.perf_counter()
    try:
        resp = session.get(URL, timeout=10)
        status = resp.status_code
    except requests.RequestException:
        status = None
    elapsed = time.perf_counter() - start
    return status, elapsed

def load_test(num_requests, max_workers):
    # Use a Session for connection pooling
    session = requests.Session()

    # Record wall-clock time for the entire batch
    batch_start = time.perf_counter()

    # Fire off all requests
    with concurrent.futures.ThreadPoolExecutor(max_workers=max_workers) as executor:
        # Pass the same session to each worker
        futures = [executor.submit(send_request, session) for _ in range(num_requests)]
        results = [f.result() for f in concurrent.futures.as_completed(futures)]

    total_time = time.perf_counter() - batch_start

    # Separate out successes, errors, and collect timings
    success_times = [rt for status, rt in results if status == 200]
    total_requests = len(results)
    success_count = len(success_times)
    error_count = total_requests - success_count

    # Benchmarks
    avg_resp_time = sum(success_times) / success_count if success_count else 0
    peak_resp_time = max(success_times) if success_times else 0
    rps = total_requests / total_time if total_time > 0 else float('inf')

    # Report
    print(f"Total requests sent:       {total_requests}")
    print(f"Successful (200):       {success_count}")
    print(f"Failed (non-200/err):   {error_count}")
    print(f"Total wall-clock time:      {total_time:.2f} sec")
    print(f"Requests per second (RPS):  {rps:.2f}")
    print(f"Average resp. time (200):   {avg_resp_time:.4f} sec")
    print(f"Peak resp. time (200):      {peak_resp_time:.4f} sec")

if __name__ == "__main__":
    load_test(NUM_REQUESTS, MAX_WORKERS)
