# dataMonitor — Project Guide

## 프로젝트 개요

Visual Studio 기반 C++ 데이터 모니터링 POC. 센서 데이터를 JSON 파일로 생성·저장하고 모니터링하는 콘솔 애플리케이션이다.

- **언어**: C++20
- **빌드 시스템**: MSBuild (Visual Studio 2022, toolset v145)
- **플랫폼**: Windows x64, Console Application
- **솔루션 파일**: `dataMonitor.slnx`
- **프로젝트 파일**: `dataMonitor/dataMonitor.vcxproj`

## 소스 구조

```
dataMonitor/
├── main.cpp                  # 진입점
├── DummyDataGenerator.cpp/h  # 더미 센서 데이터 생성
├── JsonWriter.cpp/h          # JSON 직렬화·파일 출력
└── output/                   # 생성된 JSON 파일 저장 위치 (gitignore 제외)
```

## JSON 데이터 스키마

출력 파일명 형식: `dummy_YYYYMMDD_HHMMSS.json`

```json
{
  "metadata": {
    "generated_at": "ISO 8601 UTC",
    "record_count": 100,
    "schema_version": "1.0"
  },
  "records": [
    {
      "id": "UUID v4",
      "timestamp": "ISO 8601 UTC",
      "value": 58.22,
      "tag": "TEMP_SENSOR_01",
      "status": "OK"
    }
  ]
}
```

### 센서 태그 종류

| 태그 패턴 | 설명 |
|---|---|
| `TEMP_SENSOR_xx` | 온도 센서 |
| `HUMIDITY_SENSOR_xx` | 습도 센서 |
| `FLOW_SENSOR_xx` | 유량 센서 |
| `PRESSURE_SENSOR_xx` | 압력 센서 |

### 상태값

- `OK` — 정상
- `WARN` — 경고 (임계값 근접)
- `ERROR` — 오류 (임계값 초과)

## 빌드

Visual Studio에서 솔루션 열기 → Debug/Release x64 선택 → 빌드(Ctrl+Shift+B).

MSBuild CLI:
```
msbuild dataMonitor/dataMonitor.vcxproj /p:Configuration=Debug /p:Platform=x64
```

빌드 결과물: `x64/Debug/dataMonitor.exe`

## 실행

```
x64\Debug\dataMonitor.exe
```

생성된 JSON 파일은 `dataMonitor/output/` 에 저장된다.

## 개발 지침

- JSON 직렬화는 외부 라이브러리 없이 직접 구현한다 (POC 단계).
- 새 센서 타입 추가 시 `DummyDataGenerator`에서 태그 목록만 확장한다.
- `status` 판단 로직(임계값)은 `DummyDataGenerator` 내에서 관리한다.
- 출력 디렉터리(`output/`)는 git에서 제외되어 있으므로 실행 전 폴더가 존재하는지 확인한다.
- C++20 표준을 사용하므로 `std::format`, structured bindings 등 모던 C++ 기능을 적극 활용한다.
