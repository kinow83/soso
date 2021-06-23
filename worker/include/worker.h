#ifndef __SOSO_WORKER_H__
#define __SOSO_WORKER_H__

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <stdint.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

namespace soso {

class WorkerManager;
class Worker;
class Job;

/**
 * @brief 작업 처리 타입
 */
enum JOB_QUEUE_TYPE {
  /// 여러 worker thread가 job을 처리함
  MULTI_WORKER = 0,
  /// 하나의 worker thread가 job을 처리함
  SINGLE_WORKER,
  /// { return ++_completed_jobs; }
  JOB_QUEUE_TYPE_SIZE
};

/**
 * @brief job(작업) 처리 함수 타입
 */
using job_handler_t =
    std::function<void(std::shared_ptr<Worker>, std::shared_ptr<Job>)>;

using worker_init_t = std::function<void(std::shared_ptr<Worker>)>;

using thread_handler_t = std::function<void(
    std::shared_ptr<Worker>, std::deque<std::shared_ptr<Job>> *,
    std::shared_ptr<std::mutex>, std::shared_ptr<std::condition_variable>)>;
/**
 * @brief job(작업)를 처리하기 worker(작업)를 관리하기 위한 객체
 */
class Worker {
private:
  /// worker ID
  std::size_t _worker_id;
  /// 처리 완료된 작업 수
  std::size_t _completed_jobs;
  /// 작업 관리자
  WorkerManager *_worker_manager;
  /// worker(작업자) thread 종료되었는지 여부
  bool _running = true;
  /// 작업 타입 (single, multi)
  JOB_QUEUE_TYPE _job_type;
  ///
  std::shared_ptr<void> _ctx;

public:
  /**
   * @brief Worker 생성자
   * @param worker_id worker ID
   */
  Worker(WorkerManager *worker_manager, //
         int worker_id,                 //
         JOB_QUEUE_TYPE job_type);

  std::shared_ptr<void> getCTX();

  void setCTX(std::shared_ptr<void> ctx);

  /**
   * @brief 작업 처리 타입
   * @return JOB_QUEUE_TYPE
   */
  JOB_QUEUE_TYPE getJobType();

  /**
   * @brief 작업 관리자 반환WorkerManager
   * @return const WorkerManager*
   */
  const WorkerManager *getWorkerManager();

  /**
   * @brief worker thread 종료 여부
   * @return true
   * @return false
   */
  bool finished();

  /**
   * @brief worker(작업자) thread를 종료하라고 알림
   * @return true
   * @return false
   */
  void terminate();

  /**
   * @brief worker ID를 반환
   * @return std::size_t
   */
  std::size_t getWorkerID();

  /**
   * @brief worker가 완료 처리한 job(작업) 수를 반환
   * @return std::size_t
   */
  std::size_t getCompletedJobs();

  /**
   * @brief worker가 완료 처리한 job(작업) 수 1 증가
   */
  std::size_t incCompletedJobs();
};

/**
 * @brief 작업 객체
 */
class Job {
private:
  /// 작업 처리 함수
  job_handler_t _handler;
  /// job(작업)을 할당 받은 작업자(worker) ID
  std::size_t _worker_id = 0;
  /// job(작업) ID
  std::size_t _job_id = 0;
  /// 작업 처리를 특정 worker에게 할당 여부
  bool _affinity;

public:
  /**
   * @brief Job 생성자
   * @param name 작업 이름
   * @param handler 작업 처리 함수
   * @param affinity 작업 처리 특정 worker에게 할당 할지 여부
   */
  Job(size_t job_id, job_handler_t &handler, bool affinity);

  /**
   * @brief 작업처리 affinity 여부
   * @return true
   * @return false
   */
  bool affinity();

  /**
   * @brief 작업 처리 함수 반환
   * @return job_handler_t
   */
  job_handler_t getHandler();

  /**
   * @brief worker(작업자) ID 할당
   * @param worker_id
   */
  void setWorkerID(std::size_t worker_id);

  /**
   * @brief job(작업) ID 할당
   * @param job_id
   */
  void setJobID(std::size_t job_id);

  /**
   * @brief 할당된 worker 쓰레드 ID
   * @return std::size_t
   */
  std::size_t getWorkerID() const;

  /**
   * @brief worker(작업자) 쓰레드별 할당 받은 job 순서 번호
   * @return std::size_t
   */
  std::size_t getJobID() const;
};

/**
 * @brief worker(작업자) 스레드가 job(작업)을 처리하는 라이브러리.
 * 하나의 job이 Queue(큐)에 추가되면 여러개의 worker 중 하나의 worker가 선택되어
 * job을 처리한다. 여기서 어떠한 worker가 선택될지는 모른다.
 */
class WorkerManager {
protected:
  /// 작업 관리 이름
  std::string _name;
  /// worker(작업자)에게 할당을 요청하는 선택 대기 Queue(큐)
  std::vector<std::deque<std::shared_ptr<Job>>> _job_m_Q;
  std::deque<std::shared_ptr<Job>> _job_s_Q;
  /// worker(작업자) 선택 대기 Queue 동시 접근을 위한 lock
  std::vector<std::shared_ptr<std::mutex>> _job_m_M;
  std::shared_ptr<std::mutex> _job_s_M;
  /// worker(작업자) 선택 대기 Queue 동시 접근 변수
  std::vector<std::shared_ptr<std::condition_variable>> _job_m_CV;
  std::shared_ptr<std::condition_variable> _job_s_CV;

  /// 작업자(worker) 목록
  std::vector<std::shared_ptr<Worker>> _m_workers;
  std::shared_ptr<Worker> _s_worker;

  /// 작업자(worker) 스레드 목록
  std::vector<std::shared_ptr<std::thread>> _worker_threads;

  /// worker(작업자)가 job(작업) 요청을 대기하는 시간으로 해당 시간 안에 작업
  /// 요청이 없으면 persist handle(지속적으로 처리해야 작업)이 호출됨
  std::chrono::milliseconds _wait_for_ms;
  /// 모든 worker(작업자) thread 실행 여부
  bool _running = true;
  /// worker(작업자) 수
  std::size_t _worker_num;
  std::atomic<size_t> _job_seq;
  ///
  size_t _job_count = 0;
  ///
  worker_init_t _worker_init_handler = nullptr;
  ///
  bool _joinable = true;

protected:
  /**
   * @brief worker에게 작업 요청
   * @param name
   * @param handler
   * @param type
   * @param affinity
   */
  void addJob(const std::string &name, job_handler_t handler,
              JOB_QUEUE_TYPE type, bool affinity);

  void _run(thread_handler_t f, bool block = false);

public:
  /**
   * @brief WorkerManager 생성자
   * @param worker_num worker(작업자) 수
   * @param wait_for_ms job(작업) 용처 대기 시간 (millisecond)
   */
  WorkerManager(const std::string &name, int worker_num, int wait_for_ms = 10);

  /**
   * @brief WorkerManager 소멸자
   */
  ~WorkerManager();

  /**
   * @brief worker thread initialize
   * @param handler
   */
  void setWorkerInitialize(worker_init_t handler);

  /**
   * @brief 모든 worker(작업자) thread 종료할 것을 알림
   */
  void terminate();

  /**
   * @brief worker thread가 모두 종료 될 때 까지 대기
   */
  void join();

  std::string workerName();

  std::string report();

  /**
   * @brief single worker에게 작업 요청
   * @param name
   * @param handler
   */
  void addJobSingleWorker(const std::string &name, job_handler_t handler);

  /**
   * @brief 여러 worker 중 하나의 worker에게 작업 요청
   * @param name
   * @param handler
   * @param affinity
   */
  void addJobMultiWorker(const std::string &name, job_handler_t handler,
                         bool affinity = false);
  /**
   * @brief WorkerManager 수행
   * @param block YES인 경우 run 함수 block되어 뒤에 실행 코드가 수행 안됨\n
   * FALSE인 경우 run 함수 호출 후 바로 리턴됨
   */
  void run(bool block = false);
};

class WorkerManager2 : public WorkerManager {
public:
  WorkerManager2(const std::string &name, int worker_num, int wait_for_ms = 10)
      : WorkerManager(name, worker_num, wait_for_ms) {}

  void run(bool block = false);
};

} // namespace soso
#endif