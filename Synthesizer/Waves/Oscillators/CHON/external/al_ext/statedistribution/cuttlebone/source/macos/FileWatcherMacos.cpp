#include "Cuttlebone/FileWatcher.hpp"

#include <vector>
#include <thread>
#include <cassert>
using namespace std;

#include <CoreServices/CoreServices.h>

namespace cuttlebone {

struct FileWatcherImpl {
  vector<const char *> paths;
};

FileWatcher::FileWatcher() : impl(new FileWatcherImpl) {}
FileWatcher::~FileWatcher() { delete impl; }

void FileWatcher::watch(const char *filePath) {
  static bool hasRun = false;
  if (!hasRun) {
    hasRun = true;
    impl->paths.push_back(filePath);
  } else {
    // XXX this code is incomplete
    assert(false);
  }
}

static FileWatcher *that;
static CFRunLoopRef runLoopRef;
static FSEventStreamContext context;

static void myCallbackFunction(ConstFSEventStreamRef streamRef,
                               void *clientCallBackInfo, size_t numEvents,
                               void *eventPaths,
                               const FSEventStreamEventFlags eventFlags[],
                               const FSEventStreamEventId eventIds[]) {
  // char **paths = (char **)eventPaths;  // cast to stop warning

  for (int i = 0; i < numEvents; i++) {
    if (eventFlags[i] & kFSEventStreamEventFlagItemModified) {
      that->onModify((const char *)clientCallBackInfo);
    }
    // else {
    //  printf("non-modify event on %s\n", paths[i]);
    //}
  }
}

void FileWatcher::start() {
  that = this;

  // TBD spawn a thread for each file to watch

  // CFStringRef mypath = CFSTR("/tmp/foo");
  CFStringRef mypath = CFStringCreateWithCString(
      NULL, impl->paths[0], kCFStringEncodingASCII /* kCFStringEncodingUTF8 */);

  memset(&context, 0, sizeof(context));
  context.info = (void *)impl->paths[0];
  //  context.version = 0;
  //  context.retain = NULL;
  //  context.release = NULL;
  //  context.copyDescription = NULL;

  CFArrayRef pathsToWatch =
      CFArrayCreate(NULL, (const void **)&mypath, 1, NULL);
  FSEventStreamRef stream;
  CFAbsoluteTime latency = 0.003; /* Latency in seconds */

  stream = FSEventStreamCreate(
      NULL, &myCallbackFunction, &context,
      //(FSEventStreamContext *)impl->paths[0],
      pathsToWatch, kFSEventStreamEventIdSinceNow, /* Or a previous event ID */
      latency, kFSEventStreamCreateFlagFileEvents);

  runLoopRef = CFRunLoopGetCurrent();

  FSEventStreamScheduleWithRunLoop(stream, runLoopRef, kCFRunLoopDefaultMode);

  if (!FSEventStreamStart(stream)) {
    printf("FAIL!\n");
    return;
  }

  CFRunLoopRun();
}

void FileWatcher::stop() { CFRunLoopStop(runLoopRef); }

}  // cuttlebone
