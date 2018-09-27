/* Програма за ESP8266 за комуникация с Arduino UNO и изпращане 
 * на данните в реално време към Firebase Realtime Database.
 * Изготвено от Анастасия Генова през 9.2018. 
*/

#include <ESP8266WiFi.h>                // Библиотека, съдържаща базов код за управление на ESP8266 
#include <FirebaseArduino.h>            // Библиотека, съдържаща функции за осъществяване на достъп до Firebase Realtime Database

// Авторизационни данни за Firebase
#define FIREBASE_HOST "*********.firebaseio.com"                  
#define FIREBASE_AUTH "*************************"

// Авторизационни данни за WiFi мрежа
#define WIFI_SSID "***********"     // Име на WiFi мрежа
#define WIFI_PASSWORD "************"// Парола за горната WiFi мрежа

char buf[80];                           // Буфер със символи за входящите съобщения от Arduino UNO с максимална големина 80 символа

void setup()
{
  Serial.begin(9600);                   // Инициализиране на връзка Serial 

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // Свързване с WiFi

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); // Свързване с Firebase след установена WiFi връзка
  Firebase.set("WiFi-Connect", "Connected");    // Потвърждение за свързване с Firebase
}

void loop()
{
 if (readline(Serial.read(), buf, 80) > 0) {       // Четем съобщение от входящия буфер, ако има такова, и го слагаме в buf[]
    Firebase.push("Average EMF Reading", buf);     // Изпращаме данните към Firebase Realtime Database
  }
}

/* Код за приемане на съобщения през серийна комуникация. 
 * Кодът е първоначално създаден от Majenko Technologies и е използван в тази програма
 * директно и без промени. 
 * Страница на Majenko Technologies: https://majenko.co.uk/blog/reading-serial-arduino
 * 
 * Параметри
 * ...................................................................................
 * readch   = символ, който искаме да прочетем като част от съобщение
 * buffer   = буфер, в който да съхраним съобщението щом го прочетем
 *            символът * преди параметъра символизира, че получаваме 
 *            указател (pointer) към променлива. Това ни позволява да 
 *            променяме съдържанието на променливата от тази функция
 *            и да четем съдържанието на същата променлива от друга функция. 
 * len      = дължината на входящия буфер
*/
int readline(int readch, char *buffer, int len) {
    static int pos = 0; // Текущата позиция в буфера
    int rpos;           // Брой прочетени символи

    if (readch > 0) {         // За всеки символ, който е във входящия буфер на устройството...
        switch (readch) {     // Проверяваме какъв е символът
            case '\r':        // Игнорираме "carriage return character"
                break;
            case '\n':        // Нов ред символизира край на съобщението
                rpos = pos;   // Броят прочетени символи се приравнява на текущата позиция в буфера
                pos = 0;      // Нулираме текущата позиция в буфера за следващото съобщение
                return rpos;  // Връщаме като резултат на функцията броя символи, които сме прочели
            default:          // Ако не е нов ред и не е "carriage return character"...
                if (pos < len-1) {            // Ако сме сме достигнали края на буфера (има място) добавяме текущия символ към него
                    buffer[pos++] = readch;   // Текущата позиция се приравнява на текущия символ 
                                              // и после се увеличава с 1
                    buffer[pos] = 0;          // Следващата позиция в буфера се приравнява на 0
                }
        }
    }
    return 0;                 // Връщаме 0 като резултат на функцията, което дава false в проверката в loop()
}