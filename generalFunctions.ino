boolean checkButton(byte input)
{
  if (digitalRead(buttonPin[input]) == 0)
  { // se o botao for pressionado
    if (millis() - previousDebounceTimeButtonHIGH[input] >= 100)
    { // se o botao realmente foi pressionado
      previousDebounceTimeButtonLOW[input] = millis();
      return true;
    }
    else
    { // se foi apenas um bounce
      // Serial.print("botao pressionado: ");
      // Serial.println( millis() - previousDebounceTimeButtonHIGH[input]);
      return false;
    }
  }
  else
  { // se o botao for solto
    if (millis() - previousDebounceTimeButtonLOW[input] >= 100)
    { // se o botao realmente foi solto
      previousDebounceTimeButtonHIGH[input] = millis();
      return false;
    }
    else
    { // se foi apenas um bounce
      // Serial.print("botao solto: ");
      // Serial.println( millis() - previousDebounceTimeButtonLOW[input]);
      return true;
    }
  }
}