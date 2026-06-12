# Dummy Data 생성 & DB 추가 PoC

테스트용 더미 데이터를 생성하고, 생성된 데이터를 DB에 저장하는 기능을 검증하는 PoC(Proof of Concept) 프로젝트입니다.  
Visual Studio 2022 / C++17 기반으로 구현되었으며, **Debug 빌드**에서는 gmock 단위 테스트를, **Release 빌드**에서는 CLI UI를 제공합니다.

---

## 목차

- [프로젝트 구조](#프로젝트-구조)
- [아키텍처](#아키텍처)
- [빌드 환경](#빌드-환경)
- [빌드 및 실행](#빌드-및-실행)
- [Debug 모드 — gmock 테스트](#debug-모드--gmock-테스트)
- [Release 모드 — CLI UI](#release-모드--cli-ui)
- [주요 클래스 설명](#주요-클래스-설명)
- [확장 방향](#확장-방향)

---

## 프로젝트 구조

```
DummyDataGenerator-JH/
├── DummyDataGenerator-JH.sln          # Visual Studio 솔루션
├── packages/
│   └── gmock.1.11.0/                  # NuGet gmock 패키지 (소스 포함)
└── DummyDataGenerator-JH/
    ├── DataRecord.h                   # 공유 타입 정의 (Record, RecordSet)
    ├── IDatabase.h                    # DB 인터페이스
    ├── IDummyDataGenerator.h          # 더미 데이터 생성 인터페이스
    ├── DummyDataGenerator.h/.cpp      # 랜덤 더미 데이터 생성 구현체
    ├── InMemoryDatabase.h/.cpp        # 메모리 기반 DB 구현체 (DB 대역)
    ├── DummyDataService.h/.cpp        # 서비스 레이어 (생성 + 저장 조합)
    ├── Tests.h                        # gmock 테스트 케이스 모음
    ├── main.cpp                       # 진입점 (Debug/Release 분기)
    └── packages.config                # NuGet 패키지 참조 설정
```

---

## 아키텍처

```
┌─────────────────────────────────────────────────────┐
│                   main.cpp                          │
│   #ifdef _DEBUG → 테스트 실행                        │
│   #else         → CLI UI 실행                       │
└────────────────────┬────────────────────────────────┘
                     │
          ┌──────────▼──────────┐
          │  DummyDataService   │  ← 서비스 레이어
          └───┬─────────────┬───┘
              │             │
  ┌───────────▼──┐   ┌──────▼──────────────┐
  │  IDatabase   │   │ IDummyDataGenerator  │
  └───────┬──────┘   └──────────┬───────────┘
          │                     │
  ┌───────▼──────┐   ┌──────────▼───────────┐
  │InMemoryDatabase│ │  DummyDataGenerator   │
  │ (메모리 저장)  │ │  (랜덤 데이터 생성)    │
  └──────────────┘  └───────────────────────┘

  [Debug 빌드 전용]
  MockDatabase / MockDummyDataGenerator  ← gmock Mock 클래스
```

의존 관계는 항상 **인터페이스(`IDatabase`, `IDummyDataGenerator`)** 를 향하므로, 실제 DB 드라이버 교체 시 구현체만 변경하면 됩니다.

---

## 빌드 환경

| 항목 | 내용 |
|------|------|
| IDE | Visual Studio 2022 (v143 툴셋) |
| 언어 표준 | C++17 |
| 플랫폼 | Win32 / x64 |
| 테스트 프레임워크 | Google Mock 1.11.0 (NuGet, 소스 형태 포함) |
| 외부 의존성 | 없음 (STL만 사용) |

---

## 빌드 및 실행

### 1. 솔루션 열기

```
DummyDataGenerator-JH.sln
```

Visual Studio에서 솔루션 파일을 열면 NuGet 패키지(`gmock.1.11.0`)가 `packages/` 폴더에 이미 포함되어 있어 별도 복원 불필요합니다.

### 2. 구성 선택 및 빌드

| 목적 | 구성 선택 |
|------|-----------|
| gmock 단위 테스트 확인 | `Debug \| x64` |
| CLI UI 동작 확인 | `Release \| x64` |

`Ctrl+Shift+B` 로 빌드 후 `Ctrl+F5` 로 실행합니다.

---

## Debug 모드 — gmock 테스트

`_DEBUG` 매크로가 정의된 Debug 빌드에서는 `Tests.h`에 정의된 모든 테스트 케이스가 자동 실행됩니다.

### 실행 결과 예시

```
================================================
  DummyDataGenerator PoC  [DEBUG / TEST MODE]
================================================

[==========] Running 11 tests from 2 test suites.
[----------] 6 tests from DummyDataServiceTest
[ RUN      ] DummyDataServiceTest.InitializeCallsConnect
[       OK ] DummyDataServiceTest.InitializeCallsConnect (0 ms)
[ RUN      ] DummyDataServiceTest.InitializeReturnsFalseOnFailure
[       OK ] DummyDataServiceTest.InitializeReturnsFalseOnFailure (0 ms)
[ RUN      ] DummyDataServiceTest.GenerateAndStoreInsertsAllRecords
[       OK ] DummyDataServiceTest.GenerateAndStoreInsertsAllRecords (0 ms)
[ RUN      ] DummyDataServiceTest.GenerateAndStoreCountsOnlySuccessfulInserts
[       OK ] DummyDataServiceTest.GenerateAndStoreCountsOnlySuccessfulInserts (0 ms)
[ RUN      ] DummyDataServiceTest.FetchAllDelegatesToDatabase
[       OK ] DummyDataServiceTest.FetchAllDelegatesToDatabase (0 ms)
[ RUN      ] DummyDataServiceTest.ShutdownCallsDisconnect
[       OK ] DummyDataServiceTest.ShutdownCallsDisconnect (0 ms)
[----------] 5 tests from DummyDataGeneratorTest
[ RUN      ] DummyDataGeneratorTest.GeneratesExactCount
[       OK ] DummyDataGeneratorTest.GeneratesExactCount (0 ms)
[ RUN      ] DummyDataGeneratorTest.EachRecordHasRequiredFields
[       OK ] DummyDataGeneratorTest.EachRecordHasRequiredFields (0 ms)
[ RUN      ] DummyDataGeneratorTest.EmailContainsAtSymbol
[       OK ] DummyDataGeneratorTest.EmailContainsAtSymbol (0 ms)
[ RUN      ] DummyDataGeneratorTest.AgeIsInValidRange
[       OK ] DummyDataGeneratorTest.AgeIsInValidRange (0 ms)
[ RUN      ] DummyDataGeneratorTest.PhoneMatchesKoreanFormat
[       OK ] DummyDataGeneratorTest.PhoneMatchesKoreanFormat (0 ms)
[ RUN      ] DummyDataGeneratorTest.IdsAreSequential
[       OK ] DummyDataGeneratorTest.IdsAreSequential (0 ms)
[==========] 11 tests from 2 test suites ran. (X ms total)
[  PASSED  ] 11 tests.
```

### 테스트 케이스 목록

#### `DummyDataServiceTest` — 서비스 레이어 검증 (MockDatabase, MockDummyDataGenerator 사용)

| 테스트 이름 | 검증 내용 |
|-------------|-----------|
| `InitializeCallsConnect` | `Initialize()` 호출 시 DB의 `Connect()`가 정확한 인자로 호출되는지 |
| `InitializeReturnsFalseOnFailure` | DB 연결 실패 시 `Initialize()`가 `false`를 반환하는지 |
| `GenerateAndStoreInsertsAllRecords` | 생성된 레코드 수만큼 `Insert()`가 호출되는지 |
| `GenerateAndStoreCountsOnlySuccessfulInserts` | `Insert()` 실패 건은 반환 카운트에서 제외되는지 |
| `FetchAllDelegatesToDatabase` | `FetchAll()`이 DB의 `QueryAll()` 결과를 그대로 반환하는지 |
| `ShutdownCallsDisconnect` | `Shutdown()` 호출 시 DB의 `Disconnect()`가 호출되는지 |

#### `DummyDataGeneratorTest` — 생성 로직 검증 (실제 구현체 직접 테스트)

| 테스트 이름 | 검증 내용 |
|-------------|-----------|
| `GeneratesExactCount` | 요청한 개수(0, 5, 100)만큼 레코드가 생성되는지 |
| `EachRecordHasRequiredFields` | 각 레코드에 `id`, `name`, `email`, `age`, `phone` 필드가 존재하는지 |
| `EmailContainsAtSymbol` | 생성된 이메일에 `@`가 포함되는지 |
| `AgeIsInValidRange` | 나이가 18 ~ 65 범위 안인지 |
| `PhoneMatchesKoreanFormat` | 전화번호가 `010-XXXX-XXXX` 형식(13자)인지 |
| `IdsAreSequential` | ID가 1부터 순차적으로 부여되는지 |

---

## Release 모드 — CLI UI

`NDEBUG` 매크로가 정의된 Release 빌드에서는 대화형 CLI 메뉴가 실행됩니다.

### 메뉴 구성

```
================================================
  DummyDataGenerator PoC  [RELEASE / CLI MODE]
================================================
[DB] Connected  : memory://localhost/testdb

--- Menu ---
 1. Generate & store dummy data
 2. View all stored records
 3. Clear all records
 4. Exit
Select >
```

| 메뉴 | 동작 |
|------|------|
| `1` | 생성할 레코드 수를 입력하면 더미 데이터를 생성하고 메모리 DB에 저장 |
| `2` | 저장된 전체 레코드를 표 형태로 출력 |
| `3` | 저장된 모든 레코드를 초기화 (DB 재연결) |
| `4` | 프로그램 종료 |

### 데이터 조회 화면 예시

```
ID    Name                  Email                              Age   Phone
-------------------------------------------------------------------------------------
1     Alice Smith           alice.smith@gmail.com              28    010-3821-4756
2     Bob Johnson           bob.johnson@yahoo.com              45    010-9031-2847
3     Grace Williams        grace.williams@hotmail.com         33    010-5512-9034
...

Total: 10 record(s).
```

### 생성되는 더미 데이터 필드

| 필드 | 설명 | 예시 |
|------|------|------|
| `id` | 배치 내 순번 (1부터 시작) | `1` |
| `name` | 랜덤 영문 이름 (이름 + 성) | `Alice Smith` |
| `email` | 이름 기반 이메일 주소 | `alice.smith@gmail.com` |
| `age` | 18 ~ 65 범위 랜덤 정수 | `30` |
| `phone` | 한국 휴대폰 번호 형식 | `010-1234-5678` |

---

## 주요 클래스 설명

### `DummyDataGenerator`
`IDummyDataGenerator`를 구현하는 실제 생성기입니다.  
C++ `<random>` (`std::mt19937`)을 사용해 매번 다른 데이터를 생성합니다.

### `InMemoryDatabase`
`IDatabase`를 구현하는 메모리 기반 DB 대역(Fake)입니다.  
`std::map<tableName, RecordSet>` 구조로 데이터를 보관하며, 실제 DB 없이도 전체 흐름을 검증할 수 있습니다.

### `DummyDataService`
생성기(`IDummyDataGenerator`)와 DB(`IDatabase`)를 조합하는 서비스 레이어입니다.  
두 인터페이스 모두 생성자 주입(DI)을 통해 전달받으므로 테스트 시 Mock으로 교체가 용이합니다.

---

## 확장 방향

이 PoC를 기반으로 다음과 같이 확장할 수 있습니다.

- **실제 DB 연결**: `IDatabase`를 구현하는 `SqliteDatabase`, `MySqlDatabase` 등 추가
- **스키마 정의**: JSON 또는 별도 설정 파일로 테이블 컬럼/타입을 정의하고 동적 생성
- **데이터 타입 확장**: 날짜, UUID, 주소, 회사명 등 다양한 더미 필드 추가
- **배치 처리**: 대량 데이터(수만 건) 생성 시 트랜잭션 및 벌크 인서트 지원
- **테스트 확장**: `InMemoryDatabase`를 활용한 통합 테스트 추가
