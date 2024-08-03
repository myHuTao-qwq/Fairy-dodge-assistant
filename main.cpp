#include <Windows.h>
#include <dwmapi.h>

#include <ctime>
#include <fstream>
#include <iostream>
#include <thread>

#include "screenshot.h"

// music notes! qwq
#define A3 220
#define C4 262
#define D4 293
#define E4 330
#define F4 349
#define G4 392
#define A4 440
#define B4 494
#define C5 523

std::atomic<bool> running(false);

const cv::Vec3f YELLOW(88.80018647f, 248.79645691f, 251.63850986f);
const cv::Vec3f RED(119.62991519f, 119.86961498f, 250.61517565f);

INPUT inputs[4] = {};

void pressKey(WORD key) {
  INPUT input = {0};
  input.type = INPUT_KEYBOARD;
  input.ki.wVk = key;
  input.ki.wScan = MapVirtualKeyA(key, MAPVK_VK_TO_VSC);
  uint result;
  result = SendInput(1, &input, sizeof(INPUT));
  // std::cout << "keydown " << result << std::endl;
  input.ki.dwFlags = KEYEVENTF_KEYUP;
  result = SendInput(1, &input, sizeof(INPUT));
  // std::cout << "keyup " << result << std::endl;
}

cv::Mat computeEuclideanDistance(const cv::Mat& img, const cv::Vec3f& color) {
  cv::Mat color_mat(img.size(), img.type(), color);

  cv::Mat diff = img - color_mat;

  std::vector<cv::Mat> channels(3);
  cv::split(diff, channels);

  cv::Mat sqr_sum = channels[0].mul(channels[0]) +
                    channels[1].mul(channels[1]) + channels[2].mul(channels[2]);

  cv::Mat dist;
  cv::sqrt(sqr_sum, dist);

  return dist;
}

std::pair<bool, bool> detect(const cv::Mat& img) {
  cv::Mat resized, img_float;

  img.convertTo(img_float, CV_32F);

  cv::Mat meanRow, meanCol;
  cv::reduce(img_float, meanRow, 1, cv::REDUCE_AVG);  // 行平均
  cv::reduce(img_float, meanCol, 0, cv::REDUCE_AVG);  // 列平均

  double dist_row_yellow, dist_col_yellow, dist_row_red, dist_col_red;

  cv::minMaxLoc(computeEuclideanDistance(meanRow, YELLOW), &dist_row_yellow);
  cv::minMaxLoc(computeEuclideanDistance(meanCol, YELLOW), &dist_col_yellow);
  cv::minMaxLoc(computeEuclideanDistance(meanRow, RED), &dist_row_red);
  cv::minMaxLoc(computeEuclideanDistance(meanCol, RED), &dist_col_red);

  // std::cout << " col_yellow: " << dist_col_yellow
  //           << " row_yellow: " << dist_row_yellow
  //           << " col_red: " << dist_col_red << " row_red: " << dist_row_red
  //           << std::endl;

  bool yellow_true, red_true;

  yellow_true = (dist_col_yellow < 100) && (dist_row_yellow < 110);
  red_true = (dist_col_red < 50) && (dist_row_red < 70);

  return std::make_pair(yellow_true, red_true);
}

void keyListener() {
  if (!RegisterHotKey(NULL, 0, MOD_ALT, 0x56)) {
    std::cerr << "Failed to register Alt+V" << std::endl;
  }
  if (!RegisterHotKey(NULL, 1, MOD_ALT, 0x58)) {
    std::cerr << "Failed to register Alt+X" << std::endl;
  }

  MSG msg = {0};
  while (GetMessage(&msg, NULL, 0, 0)) {
    if (msg.message == WM_HOTKEY) {
      switch (msg.wParam) {
        case 0:
          running = true;
          std::cout << "主人，系统已经成功启动，真是简单得让我有些无聊呢~"
                    << std::endl;
          Beep(C4, 100);
          Beep(E4, 100);
          Beep(G4, 100);
          Beep(C5, 100);
          break;
        case 1:
          running = false;
          std::cout << "主人，系统已经暂停，您确定不需要我了？哼~" << std::endl;
          Beep(C5, 100);
          Beep(G4, 100);
          Beep(E4, 100);
          Beep(C4, 100);
          break;
      }
    }
  }
}

int main() {
  system("@echo off\nchcp 65001");

  Beep(C4, 100);
  Beep(D4, 100);
  Beep(E4, 100);
  Beep(F4, 100);
  Beep(G4, 100);
  Beep(A4, 100);
  Beep(B4, 100);
  Beep(C5, 100);

  std::cout
      << "召唤万能的Fairy帮你弹刀!~\n按Alt+V启动弹刀检测, 按Alt+X停止弹刀检测"
      << std::endl;

  Screen screen;

  std::thread listener(keyListener);

  cv::Mat img;
  std::pair<bool, bool> result;
  bool y, r;
  double last_y, last_r;

  while (true) {
    if (running) {
      img = screen.getScreenshot();
      result = detect(img);

      y = result.first;
      r = result.second;

      // 当前时间
      double t = clock() / (double)CLOCKS_PER_SEC;

      // 按键操作和蜂鸣声
      if (y && (t - last_y) > 1.0) {
        pressKey(VK_SPACE);  // 按下空格键
        std::cout << " - 检测到黄条, 格挡!\n杂鱼~ 杂鱼~" << std::endl;
        last_y = t;
        Beep(A4, 100);
      } else if (r && (t - last_r) > 1.0) {
        pressKey(VK_SHIFT);  // 按下Shift键
        std::cout << " - 检测到红条, 闪避!\n杂鱼~ 杂鱼~" << std::endl;
        last_r = t;
        Beep(E4, 100);
      } else {
        continue;
      }

    } else {
      Sleep(5);
    }
  }

  return 0;
}