#include "Cuttlebone/FileWatcher.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <unistd.h>

#include <vector>
#include <cassert>
#include <iostream>
using namespace std;

#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + 16))

namespace cuttlebone {

struct FileWatcherImpl {
  vector<string> paths;
  int fd;
  int wd;
  char buffer[BUF_LEN];
  bool done;
  FileWatcherImpl() : done(false) {}
};

FileWatcher::FileWatcher() : impl(new FileWatcherImpl) {}
FileWatcher::~FileWatcher() { delete impl; }

void FileWatcher::watch(const char *filePath) {
  static bool hasRun = false;
  if (!hasRun) {
    hasRun = true;
    impl->paths.push_back(string(filePath));
  } else {
    // XXX this code is incomplete
    assert(false);
  }
}

// that->onModify((const char *)clientCallBackInfo);

void FileWatcher::start() {
  impl->fd = inotify_init();  // XXX inotify_init1(IN_NONBLOCK);

  impl->wd = inotify_add_watch(impl->fd, impl->paths[0].c_str(),
                               IN_CLOSE_WRITE /* not IN_MODIFY */);

  while (!impl->done) {
    int length = read(impl->fd, impl->buffer, BUF_LEN);

    if (length < 0) {
      perror("read");
    }

    for (int i = 0; i < length;) {
      struct inotify_event *event = (struct inotify_event *)&impl->buffer[i];
      if ((event->len == 0) &&
          (event->mask & IN_CLOSE_WRITE /* not IN_MODIFY */))
        this->onModify(impl->paths[0].c_str());
      i += EVENT_SIZE + event->len;
    }
  }
}

void FileWatcher::stop() {
  impl->done = true;
  inotify_rm_watch(impl->fd, impl->wd);
  close(impl->fd);
}

}  // cuttlebone
