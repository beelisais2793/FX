#ifndef RURATAE_UTIL_H__
#define RURATAE_UTIL_H_

#include <mutex>
#include <stack>
#include <queue>

namespace ruratae
{
  template<class T>
  class threadsafe_stack
  {
  public:
    void push(const T& v)
    {
      m_mutex.lock();
      m_stack.push(v);
      m_mutex.unlock();
    }

    bool pop(T& v)
    {
      m_mutex.lock();
      if (m_stack.size())
      {
        v = m_stack.top();
        m_stack.pop();
        m_mutex.unlock();
        return true;
      }
      m_mutex.unlock();
      return false;
    }

    bool try_pop(T& v)
    {
      if (m_mutex.try_lock())
      {
        if (m_stack.size())
        {
          v = m_stack.pop();
          m_mutex.unlock();
          return true;
        }
      }
      m_mutex.unlock();
      return false;
    }

    size_t size()
    {
      m_mutex.lock();
      size_t s = m_stack.size();
      m_mutex.unlock();
      return s;
    }

  private:
    std::mutex m_mutex;
    std::stack<T> m_stack;
  };

  template<class T>
  class threadsafe_queue
  {
  public:
    void push(const T& v)
    {
      m_mutex.lock();
      m_queue.push(v);
      m_mutex.unlock();
    }

    bool pop(T& v)
    {
      m_mutex.lock();
      if (m_queue.size())
      {
        v = m_queue.pop();
        m_mutex.unlock();
        return true;
      }
      m_mutex.unlock();
      return false;
    }

    bool try_pop(T& v)
    {
      if (m_mutex.try_lock())
      {
        if (m_queue.size())
        {
          v = m_queue.front();
          m_queue.pop();
          m_mutex.unlock();
          return true;
        }
      }
      m_mutex.unlock();
      return false;
    }

    size_t size()
    {
      m_mutex.lock();
      size_t s = m_queue.size();
      m_mutex.unlock();
      return s;
    }

  private:
    std::mutex m_mutex;
    std::queue<T> m_queue;
  };
}

#endif /* RURATAE_UTIL_H__ */
