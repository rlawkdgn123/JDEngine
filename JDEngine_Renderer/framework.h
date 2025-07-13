//framework.h
/*
framework.h(또는 프로젝트별 공통 헤더) :

	이 파일은 주로 프로젝트 자체의 공통 정의, 매크로, 전역 변수, 헬퍼 함수 및 클래스 등을 포함합니다.예를 들어, 게임 엔진의 핵심 구성 요소, 렌더링 파이프라인 관련 타입 정의, 또는 특정 애플리케이션의 전역 설정 등이 여기에 들어갈 수 있습니다.

	이 파일은 프로젝트의 특정 논리와 밀접하게 관련되어 있습니다.
*/
#pragma once


#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif
