#ifndef __SOSO_HASH_HPP__
#define __SOSO_HASH_HPP__

#include "atomic.h"
#include <assert.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace soso {

/**
 * @brief
 * @tparam T
 */
template <typename T> class Hash {
  /**
   * @brief
   */
  class Node {
  public:
    Node *prev, *next;
    T data;

    Node() { //
      prev = next = NULL;
    }
    Node(T &data) {
      prev = next = NULL;
      this->data = data;
    }
  };

private:
  ///
  atomic_t num_elements;
  ///
  long num_buckets;
  ///
  Hash<T>::Node **buckets;
  ///
  pthread_mutex_t *bucket_lock;
  ///
  pthread_mutex_t hash_table_lock;
  ///
  long *num_of_nodes_in_bucket;
  ///
  bool replace;

private:
  /**
   * @brief
   */
  long (*fnHash)(T &a);
  /**
   * @briefstd
   */
  int (*fnCompare)(T &a, T &b);
  /**
   * @brief
   */
  void (*fnDataFree)(T &a);
  /**
   * @brief
   */
  std::optional<T> (*fnDataCopy)(T &a);
  /**
   * @brief
   */
  std::string (*fnToString)(T &a);

public:
  /**
   * @brief HashTable 생성자
   * @param num_buckets 해쉬 버킷 수
   * @param fnHash 해쉬 함수
   * @param fnCompare 데이터 비교 함수
   * @param fnDataFree 데이터 메모리 해제 함수
   * @param fnDataCopy 데이터 복사 함수 (for thread safe)
   * @param fnToString 데이터 출력 함수
   * @param replace 중복 key 추가 시 데이터 교체 여부
   */
  Hash(long num_buckets,                     //
       long (*fnHash)(T &a),                 //
       int (*fnCompare)(T &a, T &b),         //
       void (*fnDataFree)(T &a),             //
       std::optional<T> (*fnDataCopy)(T &a), //
       std::string (*fnToString)(T &a),      //
       bool replace) {

    pthread_mutexattr_t lock_attr;

    assert(num_buckets != 0);
    assert(fnHash != NULL);
    assert(fnCompare != NULL);

    atomic_set(&num_elements, 0);

    this->num_buckets = num_buckets;
    this->fnHash = fnHash;
    this->fnCompare = fnCompare;
    this->fnDataFree = fnDataFree;
    this->fnDataCopy = fnDataCopy;
    this->fnToString = fnToString;
    this->replace = replace;

    // number of nodes in bucket
    num_of_nodes_in_bucket = new long[num_buckets];
    assert(num_of_nodes_in_bucket != NULL);
    for (int i = 0; i < num_buckets; i++) {
      num_of_nodes_in_bucket[i] = 0;
    }

    // buckets
    buckets = new Hash<T>::Node *[num_buckets];
    assert(buckets != NULL);
    for (int i = 0; i < num_buckets; i++) {
      buckets[i] = nullptr;
    }

    // recursive lock
    bucket_lock = new pthread_mutex_t[num_buckets];
    assert(bucket_lock != NULL);

    pthread_mutexattr_init(&lock_attr);
    pthread_mutexattr_settype(&lock_attr, PTHREAD_MUTEX_RECURSIVE);
    for (int i = 0; i < num_buckets; i++) {
      pthread_mutex_init(&bucket_lock[i], &lock_attr);
    }

    // hash table entire lock ( only one )
    pthread_mutex_init(&hash_table_lock, &lock_attr);
  }

  /**
   * @brief HashTable 소멸자
   */
  ~Hash() {
    Hash<T>::Node *cur, *next;

    if (this->fnDataFree) {
      for (long i = 0; i < num_buckets; i++) {
        cur = buckets[i];
        while (cur) {
          next = cur->next;
          this->fnDataFree(cur->data);
          delete cur;
          cur = next;
        }
      }

      delete[] buckets;
      delete[] bucket_lock;
      delete[] num_of_nodes_in_bucket;
    }
  }

  /**
   * @brief 데이터 추가
   * @param data
   * @return true
   * @return false
   */
  bool insert(T &data) {
    Hash<T>::Node *cur, *newnode;
    long index;
    int result;

    index = this->fnHash(data) % num_buckets;
    pthread_mutex_lock(&bucket_lock[index]);
    {
      cur = buckets[index];
      while (cur) {
        result = this->fnCompare(cur->data, data);
        if (result == 0) { // matched data
          if (replace == false) {
            pthread_mutex_unlock(&bucket_lock[index]);
            return false;
          }
          if (this->fnDataFree) {
            this->fnDataFree(cur->data);
          }
          cur->data = data;
          pthread_mutex_unlock(&bucket_lock[index]);
          return true;
        }
        cur = cur->next;
      }

      newnode = new Node(data);
      newnode->next = buckets[index];
      buckets[index] = newnode;
      if (newnode->next) {
        newnode->next->prev = newnode;
      }
      num_of_nodes_in_bucket[index]++;
    }
    pthread_mutex_unlock(&bucket_lock[index]);

    atomic_inc(&num_elements);
    return true;
  }

  /**
   * @brief 데이터 삭제
   * @param data
   * @return true
   * @return false
   */
  bool remove(T &data) {
    Hash<T>::Node *cur;
    long index;
    int result;

    index = this->fnHash(data) % num_buckets;
    pthread_mutex_lock(&bucket_lock[index]);
    {
      cur = buckets[index];
      while (cur) {
        result = this->fnCompare(cur->data, data);
        if (result == 0) { // matched data
          if (cur == buckets[index]) {
            buckets[index] = cur->next;
          } else {
            cur->prev->next = cur->next;
          }
          if (cur->next) {
            cur->next->prev = cur->prev;
          }

          if (this->fnDataFree) {
            this->fnDataFree(cur->data);
          }
          delete cur;
          atomic_dec(&num_elements);
          num_of_nodes_in_bucket[index]--;

          pthread_mutex_unlock(&bucket_lock[index]);
          return true;
        }
        cur = cur->next;
      }
    }
    pthread_mutex_unlock(&bucket_lock[index]);
    return false;
  }

  /**
   * @brief 데이터 제거 후 추출
   * @param data
   * @return std::optional<T>data_copy
   */
  std::optional<T> pop(T &data) {
    Hash<T>::Node *cur;
    long index;
    int result;
    T finddata;

    index = this->fnHash(data) % num_buckets;
    pthread_mutex_lock(&bucket_lock[index]);
    {
      cur = buckets[index];
      while (cur) {
        result = this->fnCompare(cur->data, data);
        if (result == 0) {
          if (cur == buckets[index]) {
            buckets[index] = cur->next;
          } else {
            cur->prev->next = cur->next;
          }
          if (cur->next) {
            cur->next->prev = cur->prev;
          }

          finddata = cur->data;
          delete cur;
          atomic_dec(&num_elements);

          num_of_nodes_in_bucket[index]--;
          pthread_mutex_unlock(&bucket_lock[index]);
          return std::make_optional(finddata);
        }
        cur = cur->next;
      }
    }
    pthread_mutex_unlock(&bucket_lock[index]);
    return std::nullopt;
  }

  /**
   * @brief 데이터 존재 여부
   * @param data
   * @return true
   * @return false
   */
  bool exist(T &data) {
    Hash<T>::Node *cur;
    long index;
    int result;

    index = this->fnHash(data) % num_buckets;
    pthread_mutex_lock(&bucket_lock[index]);
    {
      cur = buckets[index];
      while (cur) {
        result = this->fnCompare(cur->data, data);
        if (result == 0) {
          pthread_mutex_unlock(&bucket_lock[index]);
          return true;
        }
        cur = cur->next;
      }
    }
    pthread_mutex_unlock(&bucket_lock[index]);
    return false;
  }

  /**
   * @brief 데이터 복사 (복사 함수 구현되어야 함 - for thread safe)
   * @param data
   * @return std::optional<T>
   */
  std::optional<T> copy(T &data) {
    Hash<T>::Node *cur;
    long index;
    int result;
    T finddata;

    assert(this->fnDataCopy != NULL);

    index = this->fnHash(data) % num_buckets;
    pthread_mutex_lock(&bucket_lock[index]);
    {
      cur = buckets[index];
      while (cur) {
        result = this->fnCompare(cur->data, data);
        if (result == 0) {
          finddata = this->fnDataCopy(cur->data);
          pthread_mutex_unlock(&bucket_lock[index]);
          return std::make_optional(finddata);
        }
        cur = cur->next;
      }
    }
    pthread_mutex_unlock(&bucket_lock[index]);
    return std::nullopt;
  }

  /**
   * @brief 데이터 조회 - thread not safe
   * @param data
   * @return std::optional<T>
   */
  std::optional<T> find(T &data) {
    Hash<T>::Node *cur;
    long index;
    int result;
    T finddata;

    index = this->fnHash(data) % num_buckets;
    pthread_mutex_lock(&bucket_lock[index]);
    {
      cur = buckets[index];
      while (cur) {
        result = this->fnCompare(cur->data, data);
        if (result == 0) {
          finddata = cur->data;
          pthread_mutex_unlock(&bucket_lock[index]);
          return std::make_optional(finddata);
        }
        cur = cur->next;
      }
    }
    pthread_mutex_unlock(&bucket_lock[index]);
    return std::nullopt;
  }

  /**
   * @brief 해쉬 테이블 정보 디버깅
   * @param outfile
   */
  void show(const char *outfile) {
    Hash<T>::Node *cur;
    std::ofstream ofs;
    std::ostream *os;
    long node_count;

    if (!outfile) {
      os = &std::cout;
    } else {
      ofs.open(outfile, std::ofstream::out);
      os = &ofs;
    }

    *os << "[hashtable summary: " << atomic_read(&num_elements) << "]"
        << std::endl;

    for (long index = 0; index < num_buckets; index++) {
      pthread_mutex_lock(&bucket_lock[index]);
      {
        cur = buckets[index];
        node_count = num_of_nodes_in_bucket[index];
        if (!node_count) {
          pthread_mutex_unlock(&bucket_lock[index]);
          continue;
        }
        *os << "bucket[" << index << "] count: " << node_count << std::endl;
        while (cur) {
          *os << fnToString(cur->data) << std::endl;
          cur = cur->next;
        }
      }
      pthread_mutex_unlock(&bucket_lock[index]);
    }
  }

  /**
   * @brief 해쉬 테이블 전역 데이터를 순회하면서 func을 호출한다.\n
   * 내부적으로 버킷 lock을 걸고 순회하므로 thread-safe 함
   * @param func
   */
  void loop(std::function<void(T &data)> func) {
    Hash<T>::Node *cur;

    for (long index = 0; index < num_buckets; index++) {
      pthread_mutex_lock(&bucket_lock[index]);
      {
        cur = buckets[index];
        while (cur) {
          func(cur->data);
          cur = cur->next;
        }
      }
      pthread_mutex_unlock(&bucket_lock[index]);
    }
  }

#if 0
  void show(FILE *fp) {
    HashTable<T>::Node *cur;
    char desc[1024];
    char *to_str;
    long node_count;

    assert(this->fnString != NULL);
    if (!fp) {
      fp = stdout;
    }

    snprintf(desc, sizeof(desc), "[hashtable summary: %ld]\n",
             atomic_read(&num_elements));
    fwrite(desc, 1, strlen(desc), fp);

    for (long index = 0; index < num_buckets; index++) {
      pthread_mutex_lock(&bucket_lock[index]);
      {
        cur = buckets[index];

        node_count = num_of_n};
odes_in_bucket[index];

        snprintf(desc, sizeof(desc), "bucket[%ld]: %ld\n", index, node_count);
        fwrite(desc, 1, strlen(desc), fp);

        while (cur) {
          to_str = this->fnString(cur->data);
          if (cur == buckets[index]) {
            snprintf(desc, sizeof(desc), "\tnode(%s)", to_str);
          } else {
            snprintf(desc, sizeof(desc), " -> node(%s)", to_str);
          }
          free(to_str);
          fwrite(desc, 1, strlen(desc), fp);
          cur = cur->next;
        }

        if (node_count > 0) {
          fwrite("\n", 1, 1, fp);
        }
      }
      pthread_mutex_unlock(&bucket_lock[index]);
    }

    snprintf(desc, sizeof(desc), "[hashtable summary finished]\n");
    fwrite(desc, 1, strlen(desc), fp);
    fflush(fp);
  }
#endif
};
}; // namespace soso

#endif