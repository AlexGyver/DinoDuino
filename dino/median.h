class median3 {
  public:
    median3(int* newbuf) {
      buffer = newbuf;
    }
    int filter(int val) {
      buffer[counter] = val;
      if (++counter > 2) counter = 0;

      int a = buffer[0];
      int b = buffer[1];
      int c = buffer[2];
      int middle;

      if ((a <= b) && (a <= c)) {
        middle = (b <= c) ? b : c;
      }
      else {
        if ((b <= a) && (b <= c)) {
          middle = (a <= c) ? a : c;
        }
        else {
          middle = (a <= b) ? a : b;
        }
      }
      return middle;
    }
  private:
    int* buffer;
    byte counter;
};

class medianN {
  public:
    medianN(int* newbuf, int reads) {
      buffer = newbuf;
      NUM_READ = reads;
    }
    int filter(int val) {
      buffer[counter] = val;
      if ((counter < NUM_READ - 1) and (buffer[counter] > buffer[counter + 1])) {
        for (int i = counter; i < NUM_READ - 1; i++) {
          if (buffer[i] > buffer[i + 1]) {
            int buff = buffer[i];
            buffer[i] = buffer[i + 1];
            buffer[i + 1] = buff;
          }
        }
      } else {
        if ((counter > 0) and (buffer[counter - 1] > buffer[counter])) {
          for (int i = counter; i > 0; i--) {
            if (buffer[i] < buffer[i - 1]) {
              int buff = buffer[i];
              buffer[i] = buffer[i - 1];
              buffer[i - 1] = buff;
            }
          }
        }
      }
      if (++counter >= NUM_READ) counter = 0;
      return buffer[(int)NUM_READ / 2];
    }
  private:
    int* buffer;
    byte counter;
    int NUM_READ;
};
