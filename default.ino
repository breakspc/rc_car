#define MOTOR_A_SPD 10  // 모터A의 속도를 결정하는 PWM 핀
#define MOTOR_A_DIR 12   // 모터A의 방향을 결정하는 디지털 핀 (원문에는 MOTOR_A_DIR_PIN으로 되어 있지만, 일반적으로 DIR로 줄여서 사용하며, motor_drive() 함수에 맞춰 수정했습니다.)
#define MOTOR_B_SPD 11  // 모터B의 속도를 결정하는 PWM 핀
#define MOTOR_B_DIR 13  // 모터B의 방향을 결정하는 디지털 핀 (원문에는 MOTOR_B_DIR_PIN으로 되어 있지만, motor_drive() 함수에 맞춰 수정했습니다.)
#define A_BAL 1         // 모터A의 균형 계수 (속도 보정값)
#define B_BAL 1         // 모터B의 균형 계수 (속도 보정값)
#define M_SPEED 100     // 모터 초기 기본 속도 (PWM 값, 0~255)

// 모터의 회전 방향 및 속도 전역 변수

//unsigned char m_a_dir = 0, m_b_dir = 0; // 0 또는 1 (방향 설정)
unsigned char m_a_spd = 0, m_b_spd = 0; // PWM 속도 값 (0~255)
boolean m_a_dir = 0, m_b_dir = 0; // (원문 이미지에 있던 변수, 현재 코드에서는 사용되지 않음)

void setup() {
  Serial.begin(9600); // 시리얼 통신 초기화 (보율 9600)

  pinMode(MOTOR_A_DIR, OUTPUT); // 모터A 방향 핀 출력으로 설정
  pinMode(MOTOR_B_DIR, OUTPUT); // 모터B 방향 핀 출력으로 설정
  // PWM 핀은 analogWrite에서 자동으로 설정되므로 pinMode 불필요

  Serial.println("Hello!"); // 모터 작동 확인용 문자 출력
}

void loop() {
  unsigned char bt_cmd = 0; // 명령어 저장을 위한 변수

  if (Serial.available()) { // 시리얼 데이터가 입력되었을 때
    bt_cmd = Serial.read(); // 변수에 입력된 데이터 저장
    rc_ctrl_val(bt_cmd);       // 조정된 변수로 모터를 실제로 구동
    }   
  motor_drive();    // 입력된 데이터에 따라 모터 변수(방향/속도) 조정
}

// 입력된 명령어(cmd)에 따라 모터의 방향과 속도를 설정하는 함수
void rc_ctrl_val(unsigned char cmd) {
  if (cmd == 'F') { // 'F'가 입력되었을 때, 전진
    m_a_dir = 1; // 정방향
    m_b_dir = 1; // 정방향
    m_a_spd = M_SPEED * A_BAL; // 속도 + 보정값
    m_b_spd = M_SPEED * B_BAL;
  } else if (cmd == 'L') { // 'L'가 입력되었을 때, 좌회전 (오른쪽 모터만 구동)
    m_a_dir = 1; // 정방향
    m_b_dir = 1; // 정방향
    m_a_spd = 0; // 모터A 정지
    m_b_spd = M_SPEED * B_BAL; // 모터B만 움직임
  } else if (cmd == 'R') { // 'R'가 입력되었을 때, 우회전 (왼쪽 모터만 구동)
    m_a_dir = 1; // 정방향
    m_b_dir = 1; // 정방향
    m_a_spd = M_SPEED * A_BAL; // 모터A만 움직임
    m_b_spd = 0; // 모터B 정지
  } else if (cmd == 'C') { // 'C'가 입력되었을 때, 제자리 좌회전 (A 정, B 역)
    m_a_dir = 1; // 정방향
    m_b_dir = 0; // 역방향
    m_a_spd = M_SPEED * A_BAL;
    m_b_spd = M_SPEED * B_BAL;
  } else if (cmd == 'X') { // 'X'가 입력되었을 때, 제자리 우회전 (A 역, B 정)
    m_a_dir = 0; // 역방향
    m_b_dir = 1; // 정방향
    m_a_spd = M_SPEED * A_BAL;
    m_b_spd = M_SPEED * B_BAL;
  } else if (cmd == 'B') { // 'B'가 입력되었을 때, 후진
    m_a_dir = 0; // 역방향
    m_b_dir = 0; // 역방향
    m_a_spd = M_SPEED * A_BAL;
    m_b_spd = M_SPEED * B_BAL;
  } else if (cmd == 'P') { // 'P'가 입력되었을 때, 정지
    m_a_dir = 1; // (방향은 유지하거나 정방향으로 설정)
    m_b_dir = 1;
    m_a_spd = 0; // 속도 정지
    m_b_spd = 0; // 속도 정지
  }
}

// 설정된 변수(방향, 속도)를 모터 드라이버 핀에 출력하는 함수
void motor_drive() {
  digitalWrite(MOTOR_A_DIR, m_a_dir); // 모터A의 방향을 디지털 출력
  digitalWrite(MOTOR_B_DIR, m_b_dir); // 모터B의 방향을 디지털 출력
  analogWrite(MOTOR_A_SPD, m_a_spd); // 모터A의 속도(PWM) 출력
  analogWrite(MOTOR_B_SPD, m_b_spd); // 모터B의 속도(PWM) 출력
}
