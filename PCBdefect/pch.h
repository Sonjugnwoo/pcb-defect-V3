// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#ifndef PCH_H
#define PCH_H

// 여기에 미리 컴파일하려는 헤더 추가
#include "framework.h"
#include "opencv2/opencv.hpp"
// ****************************************************************
// YOLO LIBRARY PARAMETERS Initialize
// ****************************************************************
#define OPENCV 1 // OpenCV를 사용함을 명시
#define USE_DARKNET 1 // YOLO(Darknet) 라이브러리 사용
#define HAVE_STRUCT_TIMESPEC 1 // Windows에서 struct timespec 호환성 문제 해결용 정의
#define GPU 1 // GPU 가속 기능 사용 활성화
#define CUDNN 1 // CUDA의 CUDNN 라이브러리 사용 활성화

#include "../Yolo/yolo_v2_class.hpp"
#pragma comment(lib, "../Yolo/yolo_cpp_dll.lib")
#endif //PCH_H
