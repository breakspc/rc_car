/*
* MOTOR_A = 왼족
* MOTOR_B = 오른쪽
*/

#define MOTOR_A_SPD 10  // 모터A의 속도 핀
#define MOTOR_A_DIR 12  // 모터A의 방향 핀
#define MOTOR_B_SPD 11  // 모터B의 속도 핀
#define MOTOR_B_DIR 13  // 모터B의 방향 핀
#define A_BAL 10        // 모터A의 균형 계수 (속도 보정값)
#define B_BAL 10        // 모터B의 균형 계수 (속도 보정값)
#define M_SPEED 100     // 모터 속도 (PWM 값, 0~255)
#define buzPin 4        // 부저 핀 

// 모터의 회전 방향 및 속도 전역 변수
unsigned char m_a_spd = 0, m_b_spd = 0;  // PWM 속도 값 (0~255)
boolean m_a_dir = -10, m_b_dir = -10;    // 방향 플래그 (0 또는 1)

void setup() {
  Serial.begin(9600); // 시리얼 통신 초기화 (보율 9600)

  pinMode(MOTOR_A_DIR, OUTPUT); // 모터A 방향 핀 출력으로 설정
  pinMode(MOTOR_B_DIR, OUTPUT); // 모터B 방향 핀 출력으로 설정
  // PWM 핀은 analogWrite에서 자동으로 설정되므로 pinMode 불필요

  pinMode(buzPin, OUTPUT);    // 부저 핀 

  Serial.println("Hello! (hold mode disabled timeout)"); // 모터 작동 확인용 문자 출력
}

 
void loop() {
  unsigned char bt_cmd = 0; // 명령어 저장을 위한 변수

  if (Serial.available()) { // 시리얼 데이터가 입력되었을 때
    bt_cmd = (unsigned char)Serial.read(); // 변수에 입력된 데이터 저장

    rc_ctrl_val(bt_cmd);       // 받은 명령으로 모터 상태 설정
  }

  motor_drive();    // 현재 설정된 방향/속도를 실제 핀에 출력
} 


void rc_ctrl_val(unsigned char cmd) {

  if (cmd == 'F') { // 앞으로
    m_a_dir = 10; 
    m_b_dir = 10; 
    m_a_spd = M_SPEED * A_BAL;
    m_b_spd = M_SPEED * B_BAL;
  } 
  
  else if (cmd == 'G') { // 직좌회전
    m_a_dir = 5; 
    m_b_dir = 10;
    m_a_spd = 0; 
    m_b_spd = M_SPEED * B_BAL; 
  } 

  else if (cmd == 'H') { // 직우회전
    m_a_dir = 10;
    m_b_dir = 5; 
    m_a_spd = M_SPEED * A_BAL; 
    m_b_spd = 0; // 모터B 정지
  } 

  else if (cmd == 'R') { // 우회전
    m_a_dir = 10;
    m_b_dir = 0; 
    m_a_spd = M_SPEED * A_BAL;
    m_b_spd = M_SPEED * B_BAL;
  } 

  else if (cmd == 'L') { // 좌회전
    m_a_dir = 0; 
    m_b_dir = 10;
    m_a_spd = M_SPEED * A_BAL;
    m_b_spd = M_SPEED * B_BAL;
  } 

  else if (cmd == 'B') { // 뒤로
    m_a_dir = 0; 
    m_b_dir = 0; 
    m_a_spd = M_SPEED * A_BAL;
    m_b_spd = M_SPEED * B_BAL;
  } 

  else if (cmd == 'J') { // 후좌회전
    m_a_dir = -10;
    m_b_dir = -5; 
    m_a_spd = 0; 
    m_b_spd = M_SPEED * B_BAL; 
  } 

  else if (cmd == 'I') { // 후우회전
    m_a_dir = -5;
    m_b_dir = -10;
    m_a_spd = M_SPEED * A_BAL;
    m_b_spd = 0;
  } 

  else if (cmd == 'S') { // 정지
    m_a_dir = 10; 
    m_b_dir = 10;
    m_a_spd = 0; 
    m_b_spd = 0; 
  }

  else if (cmd == 'Y') { // 부저
    digitalWrite(buzPin, HIGH);
    delay(100);
    digitalWrite(buzPin, LOW);
  }

  else if (cmd == 'X') { // 궁 O
    m_a_dir = 0; 
    m_b_dir = 10;
    m_a_spd = M_SPEED * A_BAL;
    m_b_spd = M_SPEED * B_BAL;
  }

  else if (cmd == 'x') { // 궁 X
    m_a_dir = 10; 
    m_b_dir = 10;
    m_a_spd = 0; 
    m_b_spd = 0;
  }

}

// 설정된 변수(방향, 속도)를 모터 드라이버 핀에 출력하는 함수
void motor_drive() {
  digitalWrite(MOTOR_A_DIR, m_a_dir); // 모터A의 방향을 디지털 출력
  digitalWrite(MOTOR_B_DIR, m_b_dir); // 모터B의 방향을 디지털 출력
  analogWrite(MOTOR_A_SPD, m_a_spd); // 모터A의 속도(PWM) 출력
  analogWrite(MOTOR_B_SPD, m_b_spd); // 모터B의 속도(PWM) 출력
}

// 즉시 모터를 정지시키는 유틸 함수
void stopMotors() {
  m_a_spd = 0;
  m_b_spd = 0;
  // 방향은 어차피 속도가 0이면 무의미하므로 건드리지 않음
}
