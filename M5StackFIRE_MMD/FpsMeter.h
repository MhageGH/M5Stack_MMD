class FpsMeter {
  hw_timer_t* timer;
  float fps;
  
public:
  FpsMeter() {
    timer = timerBegin(0, 8000, true);  // period 0.1ms
  }

  // call Run() per frame
  void Run() {
    fps = 10000.0/timerRead(timer);
    timerWrite(timer, 0);
  }
  
  float GetFps() {
    return fps;
  }
};

