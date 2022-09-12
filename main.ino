#include <M5StickCPlus.h>

#define SAMPLE_PERIOD 20

const int soundNum = 8;
double doremi[soundNum] = {523.251, 587.330, 659.255, 698.456, 783.991, 880.000, 987.767, 1046.502};
bool playing = false;
String timerText = "";
String countText = "";
int pushUpCount = 0;

void taskTimer(void *pvParameters)
{
    float time = 0;
    while (true)
    {
        timerText = String(time / 10);
        timerText.remove(timerText.length() - 1);
        time++;
        vTaskDelay(100);
        if (time > 1200) // 2 minutes
        {
            M5.Beep.tone(doremi[0], 500);
            vTaskDelay(500);
            M5.Beep.end();
            playing = false;
            break;
        }
    }
    vTaskDelete(NULL);
}

void taskCount(void *pvParameters)
{
    pushUpCount = 0;
    float ax[2], ay[2], az[2];
    int hitStopCount = 1;
    float length = 0;
    do
    {

        M5.Imu.getAccelData(&ax[1], &ay[1], &az[1]);
        length = pow(ax[0] - ax[1], 2) + pow(ay[0] - ay[1], 2) + pow(az[0] - az[1], 2);
        countText = String(length);

        if (length > 0.1 && hitStopCount == 0)
        {
            pushUpCount++;
            hitStopCount = 20;
            M5.Beep.tone(doremi[(pushUpCount - 1) % 8], 500);
        }
        if (hitStopCount > 0)
        {
            hitStopCount--;
        }
        ax[0] = ax[1];
        ay[0] = ay[1];
        az[0] = az[1];
        vTaskDelay(SAMPLE_PERIOD);
    } while (playing);
    vTaskDelete(NULL);
}

void setup()
{
    M5.begin();
    M5.Imu.Init();
    M5.Lcd.setRotation(3);
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.setTextSize(10);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.drawString("Press", 0, 0);
}

void loop()
{
    M5.update();
    M5.Beep.update();
    if (M5.BtnA.wasPressed() && !playing)
    {
        M5.Lcd.fillScreen(BLACK);
        delay(1000);
        M5.Lcd.drawString("3", 0, 0);
        M5.Beep.tone(doremi[0], 500);
        delay(1000);
        M5.Lcd.drawString("2", 0, 0);
        M5.Beep.tone(doremi[0], 500);
        delay(1000);
        M5.Lcd.drawString("1", 0, 0);
        M5.Beep.tone(doremi[0], 500);
        delay(1000);
        M5.Lcd.drawString("3", 0, 0);
        M5.Beep.tone(doremi[7], 500);
        playing = true;
        M5.Lcd.fillScreen(BLACK);
        xTaskCreatePinnedToCore(taskTimer, "TaskTimer", 4096, NULL, 0, NULL, 0);
        xTaskCreatePinnedToCore(taskCount, "TaskCount", 4096, NULL, 1, NULL, 1);
    }
    M5.Lcd.drawString(timerText, 0, 0);
    M5.Lcd.drawString(String(pushUpCount), 0, 60);
}