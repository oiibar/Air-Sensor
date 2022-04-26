#include <SFE_BMP180.h>
#include <TroykaMQ.h>
#define PIN_MQ2  A1
#define PIN_MQ9  A2
#define PIN_MQ135  A3

#define PIN_MQ2  A1
MQ2 mq2(PIN_MQ2);
#define PIN_MQ9 A2
MQ9 mq9(PIN_MQ9);
#define PIN_MQ135  A3
MQ135 mq135(PIN_MQ135);

SFE_BMP180 pressure;
#define ALTITUDE 1655.0

void setup() {
  Serial.begin(9600);
  mq2.calibrate();
  mq9.cycleHeat();
  mq135.calibrate();
  Serial.print("Ro = ");
  Serial.println(mq135.getRo());
  Serial.print("Ro = ");
  Serial.println(mq2.getRo());
}

void loop() {
   Serial.print("Ratio: ");
  Serial.print(mq2.readRatio());
  // выводим значения газов в ppm
  Serial.print("\tLPG: ");
  Serial.print(mq2.readLPG());
  Serial.print(" ppm");
  Serial.print("\tMethane: ");
  Serial.print(mq2.readMethane());
  Serial.print(" ppm");
  Serial.print("\tSmoke: ");
  Serial.print(mq2.readSmoke());
  Serial.print(" ppm");
  Serial.print("\tHydrogen: ");
  Serial.print(mq2.readHydrogen());
  Serial.println(" ppm");
  delay(1000);

  if (!mq9.isCalibrated() && mq9.atHeatCycleEnd()) {
    // выполняем калибровку датчика на чистом воздухе
    mq9.calibrate();
    // при знании сопративления датчика на чистом воздухе
    // можно его указать вручную, допустим 7.2
    // mq9.calibrate(7.2);
    // выводим сопротивление датчика в чистом воздухе (Ro) в serial-порт
    Serial.print("Ro = ");
    Serial.println(mq9.getRo());
    // запускаем термоцикл
    mq9.cycleHeat();
  }
  // если прошёл интевал нагрева датчика
  // и калибровка была совершена
  if (mq9.isCalibrated() && mq9.atHeatCycleEnd()) {
    // выводим отношения текущего сопротивление датчика
    // к сопротивлению датчика в чистом воздухе (Rs/Ro)
    Serial.print("Ratio: ");
    Serial.print(mq9.readRatio());
    // выводим значения газов в ppm
    Serial.print(" LPG: ");
    Serial.print(mq9.readLPG());
    Serial.print(" ppm ");
    Serial.print(" Methane: ");
    Serial.print(mq9.readMethane());
    Serial.print(" ppm ");
    Serial.print(" CarbonMonoxide: ");
    Serial.print(mq9.readCarbonMonoxide());
    Serial.println(" ppm ");
    delay(1000);
    // запускаем термоцикл
    mq9.cycleHeat();
  }

  Serial.print("Ratio: ");
  Serial.print(mq135.readRatio());
  // выводим значения газов в ppm
  Serial.print("\tCO2: ");
  Serial.print(mq135.readCO2());
  Serial.println(" ppm");
  delay(1000);

   char status;
  double T,P,p0,a;

  Serial.println();
  Serial.print("provided altitude: ");
  Serial.print(ALTITUDE,0);
  Serial.print(" meters, ");
  Serial.print(ALTITUDE*3.28084,0);
  Serial.println(" feet");

 
  status = pressure.startTemperature();
  if (status != 0)
  {

    delay(status);

 
    status = pressure.getTemperature(T);
    if (status != 0)
    {

      Serial.print("temperature: ");
      Serial.print(T,2);
      Serial.print(" deg C, ");
      Serial.print((9.0/5.0)*T+32.0,2);
      Serial.println(" deg F");

      status = pressure.startPressure(3);
      if (status != 0)
      {

        delay(status);

 
        status = pressure.getPressure(P,T);
        if (status != 0)
        {

          Serial.print("absolute pressure: ");
          Serial.print(P,2);
          Serial.print(" mb, ");
          Serial.print(P*0.0295333727,2);
          Serial.println(" inHg");
 
          // The pressure sensor returns abolute pressure, which varies with altitude.
          // To remove the effects of altitude, use the sealevel function and your current altitude.
          // This number is commonly used in weather reports.
          // Parameters: P = absolute pressure in mb, ALTITUDE = current altitude in m.
          // Result: p0 = sea-level compensated pressure in mb
 
          p0 = pressure.sealevel(P,ALTITUDE); // we're at 1655 meters (Boulder, CO)
          Serial.print("relative (sea-level) pressure: ");
          Serial.print(p0,2);
          Serial.print(" mb, ");
          Serial.print(p0*0.0295333727,2);
          Serial.println(" inHg");
 
          // On the other hand, if you want to determine your altitude from the pressure reading,
          // use the altitude function along with a baseline pressure (sea-level or other).
          // Parameters: P = absolute pressure in mb, p0 = baseline pressure in mb.
          // Result: a = altitude in m.
 
          a = pressure.altitude(P,p0);
          Serial.print("computed altitude: ");
          Serial.print(a,0);
          Serial.print(" meters, ");
          Serial.print(a*3.28084,0);
          Serial.println(" feet\n");
        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");
 
  delay(5000);  // Pause for 5 seconds.
}
