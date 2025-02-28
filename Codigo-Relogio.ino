#include <DS1302.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Endereço do LCD I2C

const int RST = 5;  // Chip Enable
const int DAT = 6;  // Input/Output
const int CLK = 7;  // Serial Clock

DS1302 rtc(RST, DAT, CLK);

// Variáveis para armazenar valores antigos e evitar reescrita desnecessária
String ultimaLinha1 = "";
String ultimaLinha2 = "";

// Nomes abreviados dos dias da semana
const char* diasSemana[] = {"DOM", "SEG", "TER", "QUA", "QUI", "SEX", "SAB"};

void setup() {
    Serial.begin(9600);
    lcd.begin();
    lcd.backlight();

    // Testa o DS1302
    if (testaDS1302()) {
        Serial.println("DS1302 funcionando corretamente.");
        lcd.print("DS1302 OK");
    } else {
        Serial.println("Erro: DS1302 não responde.");
        lcd.print("Erro: DS1302");
        while (true); // Para o código se o DS1302 não estiver funcionando
    }

    // Prepara o relógio e desativa a proteção contra a escrita
    rtc.halt(false);
    rtc.writeProtect(false);

    // Define uma data e hora inicial (apenas na primeira vez)
    Time t(2025, 2, 5, 14, 50, 0, 2); // Ano, Mês, Dia, Hora, Minuto, Segundo, Dia da Semana
    rtc.time(t); // Define a data e hora no chip
}

void loop() {
    mostraDataHora();
    delay(1000);
}

// Função para testar a comunicação com o DS1302
bool testaDS1302() {
    rtc.halt(false); // Desativa o modo de pausa
    Time t = rtc.time(); // Tenta ler a hora

    return (t.sec < 60); // Se o segundo lido for válido, o chip está funcionando
}

// Função que mostra no LCD (e monitor série) a data e hora atual
void mostraDataHora() {
    Time t = rtc.time();

    // Formata a primeira linha (Data + Saudação)
    char linha1[17]; // "DD/MM/YY B. noite"
    String saudacao = calculaSaudacao(t.hr);
    snprintf(linha1, sizeof(linha1), "%02d/%02d/%02d %s", t.date, t.mon, t.yr % 100, saudacao.c_str());

    // Pega o dia da semana abreviado
    String diaSemana = diasSemana[t.day];

    // Formata a segunda linha (Hora no meio + Dia da Semana)
    char linha2[17]; // "  HH:MM:SS SEX  "
    snprintf(linha2, sizeof(linha2), "  %02d:%02d:%02d %s ", t.hr, t.min, t.sec, diaSemana.c_str());

    // Evita reescrever o LCD desnecessariamente
    if (ultimaLinha1 != linha1) {
        lcd.setCursor(0, 0);
        lcd.print(linha1);
        ultimaLinha1 = linha1;
    }

    if (ultimaLinha2 != linha2) {
        lcd.setCursor(0, 1);
        lcd.print(linha2);
        ultimaLinha2 = linha2;
    }

    // Mostra a data e hora no monitor série
    Serial.print("Data: ");
    Serial.print(t.date);
    Serial.print("/");
    Serial.print(t.mon);
    Serial.print("/");
    Serial.print(t.yr);
    Serial.print(" Hora: ");
    Serial.print(t.hr);
    Serial.print(":");
    Serial.print(t.min);
    Serial.print(":");
    Serial.print(t.sec);
    Serial.print(" Dia: ");
    Serial.println(diaSemana);
}

// Função que calcula a saudação com base na hora (encurtada para caber)
String calculaSaudacao(int hora) {
    if (hora >= 6 && hora < 12) {
        return "B.Dia";  // Encurtado
    } else if (hora >= 12 && hora < 18) {
        return "B.Tarde";  // Encurtado
    } else {
        return "B.Noite";  // Encurtado
    }
}
