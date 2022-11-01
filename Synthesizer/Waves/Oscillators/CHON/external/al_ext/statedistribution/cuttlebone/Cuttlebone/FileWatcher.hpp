#ifndef CUTTLEBONE_FILE_WATCHER__
#define CUTTLEBONE_FILE_WATCHER__

namespace cuttlebone {

struct FileWatcherImpl;
class FileWatcher {
  FileWatcherImpl* impl;

 public:
  FileWatcher();
  virtual ~FileWatcher();
  void start();
  void stop();
  void watch(const char* filePath);
  virtual void onModify(const char* filePath) = 0;
};

}  // cuttlebone

#endif
