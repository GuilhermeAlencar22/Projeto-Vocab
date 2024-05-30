#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <Wire.h>

// Define os pinos do módulo RFID
const int chipSelectPin = 10;
const int resetPowerDownPin = 9;
MFRC522 rfid(chipSelectPin, resetPowerDownPin);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Estrutura para armazenar o mapeamento UID -> letra
struct UidToletter {
  String uid;
  String letra;
};

// Array de mapeamentos (substitua pelas suas palavras e UIDs)
UidToletter array[] = {
  {"c345611a", "A"},
  {"5342661a", "E"},
  {"371721a", "I"},
  {"238c751a", "O"},
  {"a31f611a", "U"},
};
int tamanhoArray = sizeof(array) / sizeof(array[0]);

struct UidToword {
  String uidP;
  String palavra;
  String vogais[4];  // Ajuste para armazenar as vogais
};

UidToword arrayP[] = {
  {"a396631a", "C_D__R_", {"A", "E", "I", "A"}},
  {"932c771a", "C_V_L_", {"A", "A", "O", ""}},
  {"9439ab1e", "L__T_", {"E", "I", "E", ""}},
  {"3676f1a", "_B_C_X_", {"A", "A", "A", "I"}},
};

int tamanhoArrayP = sizeof(arrayP) / sizeof(arrayP[0]);
int gled = 2;
int rled = 3;

String palavraAtual = "";  // Armazena a palavra atualmente selecionada
String vogaisParaPreencher[4];  // Armazena as vogais para a palavra atual
int vogalIndex = 0;  // Índice para a próxima vogal a ser preenchida

void setup() {
  pinMode(gled, OUTPUT);
  pinMode(rled, OUTPUT);
  Serial.begin(9600);
  Wire.begin();
  Serial.println("Bem-vindo ao VOCAB! Aproxime o cartão");
  SPI.begin();
  rfid.PCD_Init();
 
  // Display
  lcd.init();
  lcd.setBacklight(HIGH);
  lcd.setCursor(3, 0);
  lcd.print("Bem-vindo");
  lcd.setCursor(3, 1);
  lcd.print("ao VOCAB!");
  delay(5000);
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Aproxime o");
  lcd.setCursor(4, 1);
  lcd.print("cartao");
}

void loop() {
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String uid = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      uid += String(rfid.uid.uidByte[i], HEX);
    }
    Serial.println(uid);

    // Verifica se o UID corresponde a uma palavra
    bool palavraEncontrada = false;
    for (int i = 0; i < tamanhoArrayP; i++) {
      if (uid == arrayP[i].uidP) {
        palavraAtual = arrayP[i].palavra;
        for (int j = 0; j < 4; j++) {
          vogaisParaPreencher[j] = arrayP[i].vogais[j];
        }
        vogalIndex = 0;  // Reinicia o índice das vogais
        palavraEncontrada = true;
      }
    }

    if (palavraEncontrada) {
      lcd.clear();
      lcd.print(palavraAtual);
    } else {
      // Verifica se o UID corresponde a uma vogal
      for (int i = 0; i < tamanhoArray; i++) {
        if (uid == array[i].uid) {
          String letra = array[i].letra;
          if (vogalIndex < 4 && vogaisParaPreencher[vogalIndex] == letra) {
            digitalWrite(gled, HIGH);
            delay(1000);
            digitalWrite(gled, LOW);
            // Preenche a próxima vogal na palavra
            for (int k = 0; k < palavraAtual.length(); k++) {
              if (palavraAtual.charAt(k) == '_') {
                palavraAtual.setCharAt(k, letra.charAt(0));
                vogalIndex++;
                lcd.clear();
                lcd.print(palavraAtual);
                break;
              }
            }
          }else{
            digitalWrite(rled, HIGH);
            delay(1000);
            digitalWrite(rled, LOW);
          } 
          }
        }
      }
    }
  }
