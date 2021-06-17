#ifndef __SOSO_WORKER_HPP__
#define __SOSO_WORKER_HPP__

#include <chrono>
#include <condition_variable>
#include <deque>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <stdint.h>
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
  JOB_QUEUE_TYPE_SIZE
};

/**
 * @brief job(작업) 처리 함수 타입
 */
using job_handler_t =
    std::function<void(std::shared_ptr<Worker>, std::shared_ptr<Job>)>;

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

public:
  /**
   * @brief Worker 생성자
   * @param worker_id worker ID
   */
  Worker(WorkerManager *worker_manager, //
         int worker_id,                 //
         JOB_QUEUE_TYPE job_type) {

    _worker_manager = worker_manager;
    _worker_id = worker_id;
    _job_type = job_type;
    _completed_jobs = 0;
  }

  /**
   * @brief 작업 처리 타입
   * @return JOB_QUEUE_TYPE
   */
  JOB_QUEUE_TYPE getJobType() { return _job_type; }

  /**
   * @brief 작업 관리자 반환
   * @return const WorkerManager*
   */
  const WorkerManager *getWorkerManager() { return _worker_manager; }

  /**
   * @brief worker thread 종료 여부
   * @return true
   * @return false
   */
  bool finished() { return (_running == false); }

  /**
   * @brief worker(작업자) thread를 종료하라고 알림
   * @return true
   * @return false
   */
  void terminate() { _running = false; }

  /**
   * @brief worker ID를 반환
   * @return std::size_t
   */
  std::size_t getWorkerID() { return _worker_id; }

  /**
   * @brief worker가 완료 처리한 job(작업) 수를 반환
   * @return std::size_t
   */
  std::size_t getCompletedJobs() { return _completed_jobs; }

  /**
   * @brief worker가 완료 처리한 job(작업) 수 1 증가
   */
  std::size_t incCompletedJobs() { return ++_completed_jobs; }
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
  /// 작업 이름
  std::string _name;
  /// 작업 객체 해쉬 값
  std::size_t _hashcode;
  /// 작업 처리를 특정 worker에게 할당 여부
  bool _affinity;

public:
  /**
   * @brief Job 생성자
   * @param name 작업 이름
   * @param handler 작업 처리 함수
   * @param affinity 작업 처리 특정 worker에게 할당 할지 여부
   */
  Job(const std::string &name, job_handler_t &handler, bool affinity) { //
    _name = name;
    _handler = handler;
    _hashcode = std::hash<std::string>{}(name);
    _affinity = affinity;
  }

  /**
   * @brief 작업처리 affinity 여부
   * @return true
   * @return false
   */
  bool affinity() { return _affinity; }

  std::size_t jobHashCode() { return _hashcode; }

  /**
   * @brief 작업 처리 함수 반환
   * @return job_handler_t
   */
  job_handler_t getHandler() { return _handler; }

  /**
   * @brief worker(작업자) ID 할당
   * @param worker_id
   */
  void setWorkerID(std::size_t worker_id) { _worker_id = worker_id; }

  /**
   * @brief job(작업) ID 할당
   * @param job_id
   */
  void setJobID(std::size_t job_id) { _job_id = job_id; }

  /**
   * @brief 할당된 worker 쓰레드 ID
   * @return std::size_t
   */
  std::size_t getWorkerID() const { return _worker_id; }

  /**
   * @brief worker(작업자) 쓰레드별 할당 받은 job 순서 번호
   * @return std::size_t
   */
  std::size_t getJobID() const { return _job_id; }
};

/**
 * @brief worker(작업자) 스레드가 job(작업)을 처리하는 라이브러리.
 * 하나의 job이 Queue(큐)에 추가되면 여러개의 worker 중 하나의 worker가 선택되어
 * job을 처리한다. 여기서 어떠한 worker가 선택될지는 모른다.
 */
class WorkerManager {
public:
private:
  /// worker(작업자)에게 할당을 요청하는 선택 대기 Queue(큐)
  std::deque<std::shared_ptr<Job>> _job_Q[JOB_QUEUE_TYPE_SIZE];
  /// worker(작업자) 선택 대기 Queue 동시 접근을 위한 lock
  std::mutex _job_Q_mutex[JOB_QUEUE_TYPE_SIZE];
  /// worker(작업자) 선택 대기 Queue 동시 접근 변수
  std::condition_variable _job_Q_cv[JOB_QUEUE_TYPE_SIZE];

  /// 작업자(worker) 목록
  std::vector<std::shared_ptr<Worker>> _workers;
  /// 작업자(worker) 스레드 목록
  std::vector<std::shared_ptr<std::thread>> _worker_threads;

  /// worker(작업자)가 job(작업) 요청을 대기하는 시간으로 해당 시간 안에 작업
  /// 요청이 없으면 persist handle(지속적으로 처리해야 작업)이 호출됨
  std::chrono::milliseconds _wait_for_ms;
  /// 모든 worker(작업자) thread 실행 여부
  bool _running = true;
  ///
  std::size_t _worker_num;

private:
  /**
   * @brief worker thread 동작 함수\n
   * worker(작업자) thread는 작업큐에서 job(작업)을 가져와 작업
   * 함수(job_handle)를 수행한다.
   * @param worker 작업자
   */
  void workerThreadFunc(std::shared_ptr<Worker> worker) {
    std::shared_ptr<Job> job;
    std::size_t job_id = 0;
    JOB_QUEUE_TYPE type = worker->getJobType();
    std::size_t affinity_idx;

    auto worker_manager = worker->getWorkerManager();

    while (worker_manager->_running) {
      std::unique_lock<std::mutex> lock(_job_Q_mutex[type]);

      // wait for job wakeup signal
      if (_wait_for_ms.count() == 0) {
        _job_Q_cv[type].wait(lock);
      } else {
        _job_Q_cv[type].wait_for(lock, _wait_for_ms);
      }

      while (!_job_Q[type].empty()) {
        job = _job_Q[type].front();

        /// affinity인 경우 worker ID와 작업 hashcode를 비교하여
        /// worker가 처리해야 할 작업인지 판단
        /// 현재 worker가 처리해야 할 작업이 아닌 경우 작업 큐에서 꺼내지 않고
        /// 다른 worker가 처리할 수 있도록 알려(notify)준다.
        if (job->affinity()) {
          affinity_idx = job->jobHashCode() % worker_manager->_worker_num;
          if (worker->getWorkerID() != affinity_idx) {
            _job_Q_cv[type].notify_one();
            /// 다른 worker에게 CPU를 양보한다.
            std::this_thread::yield();
            break;
          }
        }

        job->setJobID(job_id++);
        job->setWorkerID(worker->getWorkerID());
        auto handler = job->getHandler();
        handler(worker, job); // call job user defined function

        _job_Q[type].pop_front();
        worker->incCompletedJobs(); // increment completed job count
      }
    }

    /// worker thread 종료 되었음을 알림
    worker->terminate();
    pthread_exit(NULL);
    return;
  }

  /**
   * @brief worker에게 작업 요청
   * @param name
   * @param handler
   * @param type
   * @param affinity
   */
  void addJob(const std::string &name, //
              job_handler_t handler,   //
              JOB_QUEUE_TYPE type, bool affinity) {

    std::lock_guard<std::mutex> lock(_job_Q_mutex[type]);
    _job_Q[type].push_back(std::make_shared<Job>(name, handler, affinity));
    /// worker에게 작업이 추가되었음을 알림
    _job_Q_cv[type].notify_one();
  }

public:
  /**
   * @brief 모든 worker(작업자) thread 종료할 것을 알림
   * @param with_join 모든 worker가 종료될 때 까지 대기할지 여부
   */
  void terminate(bool with_join) { //
    _running = false;
    if (with_join) {
      join();
    }
  }

  /**
   * @brief worker thread가 모두 종료 될 때 까지 대기
   */
  void join() {
    for (auto worker : _workers) {
      while (true) {
        if (worker->finished()) {
          break;
        }
        std::this_thread::sleep_for(_wait_for_ms);
      }
    }
  }

  /**
   * @brief ModuleWorker 생성자
   * @param worker_num worker(작업자) 수
   * @param wait_for_ms job(작업) 용처 대기 시간 (millisecond)
   */
  WorkerManager(int worker_num, int wait_for_ms) {
    _worker_num = worker_num;
    this->_wait_for_ms = std::chrono::milliseconds(wait_for_ms);
    int i = 0;

    /// initialize job workers
    for (; i < worker_num; i++) {
      _workers.push_back( //
          std::make_shared<Worker>(this, i, MULTI_WORKER));
    }
    _workers.push_back( //
        std::make_shared<Worker>(this, i, SINGLE_WORKER));
  }

  /**
   * @brief single worker에게 작업 요청
   * @param name
   * @param handler
   */
  void addJobSingleWorker(const std::string &name, //
                          job_handler_t handler) {
    return addJob(name, handler, SINGLE_WORKER, false);
  }

  /**
   * @brief 여러 worker 중 하나의 worker에게 작업 요청
   * @param name
   * @param handler
   * @param affinity
   */
  void addJobMultiWorker(const std::string &name, //
                         job_handler_t handler,   //
                         bool affinity = false) {
    return addJob(name, handler, MULTI_WORKER, affinity);
  }

  /**
   * @brief ModuleWorker 수행
   * @param block YES인 경우 run 함수 block되어 뒤에 실행 코드가 수행 안됨\n
   * FALSE인 경우 run 함수 호출 후 바로 리턴됨
   */
  void run(bool block = false) {
    /// create job worker thread
    for (auto worker : _workers) {
      // thread create and run
      auto th = std::make_shared<std::thread>(
          std::thread(&WorkerManager::workerThreadFunc, this, worker));
      _worker_threads.push_back(th);
    }

    // wait for terminated thread
    for (auto th : _worker_threads) {
      if (block) {
        /// block thread
        (*th).join();
      } else {
        /// non block thread
        (*th).detach();
      }
    }
  }
};

} // namespace soso
#endif