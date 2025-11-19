#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EtherCard.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 4
#define CS_PIN 10


// 使用-1禁用复位引脚，并预定义I2C地址
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
static byte mymac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte Ethernet::buffer[500]; // 缩减缓冲区

void setup() {
    Serial.print("@"); // 开机时打印唯一字符
  delay(100);
 
  Serial.println("Boot Start");
  delay(1000); // 电源稳定延时
  while (!Serial); // 等待串口就绪（仅对部分支持USB的Arduino有效）
  delay(1000);
  Serial.begin(57600);
  
  // 初始化I2C总线
 Wire.begin();
Wire.setClock(100000); // 降低I2C速度
  delay(100);
  
  // 扫描I2C设备
  Serial.println("Scanning I2C...");
  for(uint8_t addr=1; addr<=127; addr++){
    Wire.beginTransmission(addr);
    if(Wire.endTransmission() == 0){
      Serial.print("Found device at 0x");
      Serial.println(addr,HEX);
    }
  }
  


// 尝试不同地址和电压模式
if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // 切换电压模式
  // 尝试0x3D或SSD1306_EXTERNALVCC
  Serial.println("OLED Init Fail: Check Address/Power");
  while(1);
}
  
  // 显示初始化状态
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Init Ethernet...");
  display.display();
  
  // 初始化以太网
  if (ether.begin(sizeof Ethernet::buffer, mymac, CS_PIN) == 0) {
    showError("Eth Init Fail");
  }
  
  // 获取DHCP
  if (!ether.dhcpSetup()) {
    showError("DHCP Fail");
  }

  // 显示IP信息
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("IP: ");
  display.print(ether.myip[0]); display.print(".");
  display.print(ether.myip[1]); display.print(".");
  display.print(ether.myip[2]); display.print(".");
  display.println(ether.myip[3]);
  display.display();
}

void loop() {
  ether.packetLoop(ether.packetReceive());
}

void showError(const char* msg) {
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("ERROR: ");
  display.println(msg);
  display.display();
  while(1);
}