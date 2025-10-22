// 2-motor 버전 (on/off 방식): 모터 A/B는 방향 핀 + ON/OFF(속도 제어 없음)로 제어
// LED 관련 코드 제거됨 (LED는 없음)
// 모터 방향 전환 시 H-브리지 보호를 위해 모터는 잠깐 끄고 방향을 바꾼 뒤 다시 켭니다.

#define MOTOR_A_SPD 10  // 모터A의 속도/enable 핀 (ON/OFF 용)
#define MOTOR_A_DIR 12  // 모터A의 방향 핀
#define MOTOR_B_SPD 11  // 모터B의 속도/enable 핀 (ON/OFF 용)
#define MOTOR_B_DIR 13  // 모터B의 방향 핀

const int buzPin = 4;  // Buzzer pin (필요 없으면 사용하지 마세요)

int valOnLevel = HIGH; // ON일때 디지털 HIGH 사용 (필요하면 analogWrite 255로 변경)

// 현재 상태 추적
int curDirA = LOW;
int curDirB = LOW;
bool curOnA = false;
bool curOnB = false;

// 안전하게 모터를 켜고 끄는 함수
void setMotorAOn(bool on) {
    if (on) digitalWrite(MOTOR_A_SPD, valOnLevel);
    else digitalWrite(MOTOR_A_SPD, LOW);
    curOnA = on;
}
void setMotorBOn(bool on) {
    if (on) digitalWrite(MOTOR_B_SPD, valOnLevel);
    else digitalWrite(MOTOR_B_SPD, LOW);
    curOnB = on;
}

// 방향 변경 시 안전 절차: 모터를 끄고 방향 변경 후 필요에 따라 켬
void safeChangeDirection(int dirA, int dirB, bool enableA, bool enableB) {
    // 먼저 모터 끔
    setMotorAOn(false);
    setMotorBOn(false);
    delay(30); // H-브리지 스위치 안전 대기 (필요시 50~100으로 증가)
    // 방향 설정
    digitalWrite(MOTOR_A_DIR, dirA);
    digitalWrite(MOTOR_B_DIR, dirB);
    curDirA = dirA;
    curDirB = dirB;
    // 모터 다시 켬 (요청된 것만)
    setMotorAOn(enableA);
    setMotorBOn(enableB);
}

void setup() {
    Serial.begin(9600);

    // 부저 초기화 (옵션)
    pinMode(buzPin, OUTPUT);
    digitalWrite(buzPin, LOW);

    // 모터 핀 설정
    pinMode(MOTOR_A_DIR, OUTPUT);
    pinMode(MOTOR_B_DIR, OUTPUT);
    pinMode(MOTOR_A_SPD, OUTPUT);
    pinMode(MOTOR_B_SPD, OUTPUT);

    // 초기 정지 상태
    digitalWrite(MOTOR_A_DIR, LOW);
    digitalWrite(MOTOR_B_DIR, LOW);
    curDirA = LOW;
    curDirB = LOW;
    setMotorAOn(false);
    setMotorBOn(false);
}

// 유틸: 현재 "직진(또는 후진) 중인지" 확인
bool movingStraight() {
    return curOnA && curOnB && (curDirA == curDirB);
}

// 유틸: 현재 정지인지
bool isStopped() {
    return !curOnA && !curOnB;
}

void loop() {
    while (Serial.available() > 0) {
        char command = Serial.read();
        if (command == '\n' || command == '\r') continue;
        Serial.println(command);

        switch(command) {
            case 'F': // 앞으로 (두 모터 동일 방향, 둘 다 ON)
                // 만약 방향이 바뀌는 경우 safeChangeDirection 사용
                if (!(curDirA == HIGH && curDirB == HIGH)) {
                    safeChangeDirection(HIGH, HIGH, true, true);
                } else {
                    // 방향은 이미 맞고, 켜져있지 않으면 켬
                    setMotorAOn(true);
                    setMotorBOn(true);
                }
                break;

            case 'B': // 뒤로 (두 모터 동일 방향, 둘 다 ON)
                if (!(curDirA == LOW && curDirB == LOW)) {
                    safeChangeDirection(LOW, LOW, true, true);
                } else {
                    setMotorAOn(true);
                    setMotorBOn(true);
                }
                break;

            case 'L': // 좌회전
                if (movingStraight()) {
                    // 직진/후진 중이면 커브: 왼쪽 모터 끄고 오른쪽 모터만 동작 -> 좌측으로 굽음
                    setMotorAOn(false); // 왼쪽 끔
                    setMotorBOn(true);  // 오른쪽 유지
                } else if (isStopped()) {
                    // 정지상태면 제자리 회전: 왼쪽 뒤로, 오른쪽 앞으로 (회전 방향은 필요에 따라 바꾸세요)
                    safeChangeDirection(LOW, HIGH, true, true); // 왼쪽 backward, 오른쪽 forward
                } else {
                    // 다른 상태(예: 이미 커브 중)이면 기본적으로 제자리 회전으로 전환
                    safeChangeDirection(LOW, HIGH, true, true);
                }
                break;

            case 'R': // 우회전
                if (movingStraight()) {
                    // 직진/후진 중이면 커브: 오른쪽 모터 끄고 왼쪽 모터만 동작 -> 우측으로 굽음
                    setMotorAOn(true);
                    setMotorBOn(false);
                } else if (isStopped()) {
                    // 정지면 제자리 회전: 왼쪽 앞으로, 오른쪽 뒤로
                    safeChangeDirection(HIGH, LOW, true, true); // 왼쪽 forward, 오른쪽 backward
                } else {
                    // 다른 상태면 제자리 회전
                    safeChangeDirection(HIGH, LOW, true, true);
                }
                break;

            case 'S': // 정지: 둘 다 끔
                setMotorAOn(false);
                setMotorBOn(false);
                break;

            case 'Y': // 부저 (옵션)
                digitalWrite(buzPin, HIGH);
                delay(200);
                digitalWrite(buzPin, LOW);
                break;

            // 속도 제어는 더 이상 사용하지 않음 — 숫자 명령 무시하거나 필요시 ON/OFF로 사용
            default:
                // 알 수 없는 명령 무시
                break;
        }
    }
}
