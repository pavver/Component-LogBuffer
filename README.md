# LogBuffer

## Короткий опис

Буфер логу працює як динамічний кільцевий буфер, реалізований механізм циклічного запису, тобто коли буфер заповнюєтся то нові данні записуются по верху старих, при цьому ураховуєтся різна довжина стрічок буферу і буфер використовуєтся максимально еффективно (без фрагментації).

## Конфігурація

У файлі './include/LogBuffer.h' можна змінити розмір буфера та максимальну кількість стрічок які можна записати в буфер

ВАЖЛИВО! якщо вам вотрібно максимально зберегти память то обираючи розмір LOG_BUFF_LENGTH треба враховувати що в буфер обовязково має поміститись мінімум дві стрічки, тобто якщо у вас максимальна довжина стрічок може бути наприклад 100 символів то ніяк не можна робити розмір буферу менше ніж 102 (з урахунком символа кінця стрічки)

```cpp
// Розмір текстового циклічного буферу лога
#define LOG_BUFF_LENGTH 4096

// Максимальна кількість окремих стрічок логу
#define LOG_MAX_LINES 128
```

коротке доповнення. LOG_BUFF_LENGTH це буквально массив символів стрічок логу, а LOG_MAX_LINES це массив з окремими обєктами які позначають початок і кінець кожної окремої стрічки, ці два параметри певним чином обмежують використання буферу, наприклад якщо записати 128 стрічок по пару символів то буфер символів не заповнится на максимум але "старі" данні всеодно буде втрачено, так само і в іншу сторону.

## Приклади і методи

Мінімальний приклад використання:

```cpp
void app_main(void)
{
  // Встановлюємо функцію для перетворення логів в буфер
  esp_log_set_vprintf(&log_toBuffer);

  while (true)
  {
    ESP_LOGI("MAIN", "Hello, world!");
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
```

тепер всі логі будуть записуватись в наш буфер, далі витягнути їх звідти можна наступними методами:

```cpp
// Витягує саму першу доступну стрічку логу
// якщо вам потрібно зчитати весь лог то потрібно виконати цей метод найпершим
LogLine *line = log_readFirst();

printf(line->GetLogString());

delete line;
```

```cpp
// Зчитує стрічку логу яка була записана самою останньою
LogLine *line = log_readLast()

printf(line->GetLogString());

delete line;
```

відобразити весь лог з буфера

```cpp
LogLine *line = log_readFirst();

while (line != nullptr)
{
  printf(line->GetLogString());

  uint16_t id = line->GetId();

  delete line;

  line = log_readNext(id);
}
```
