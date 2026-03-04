# Структура проекта dxOverlay - Quick Reference

## 📂 Структура файлов

```
dxOverlay/
├── Config.h                 ← ⭐ ВСЕ КОНСТАНТЫ ЗДЕСЬ
├── Types.h                  ← ⭐ ВСЕ ENUM'Ы И СТРУКТУРЫ
│
├── EffectSystem.h/cpp       ← 🔥 ГЛАВНЫЙ МОДУЛЬ (State Machine)
├── Window.h/cpp             ← Message loop и главное окно
├── ControlWindow.h/cpp      ← DEV панель управления
│
├── BlurEffect.h/cpp         ← Эффект: Размытие
├── OverlayErase.h/cpp       ← Эффект: Стирание
├── HalfScreenCover.h/cpp    ← Эффект: Полэкрана
├── SensitivityEffect.h/cpp  ← Эффект: Отключить мышь
├── SensitivityHighEffect.h/cpp ← Эффект: Максимум мышь
├── FpsEffect.h/cpp          ← Эффект: Низкий FPS
├── InvertMouseEffect.h/cpp  ← Эффект: Инверсия мыши
├── InvertWASDEffect.h/cpp   ← Эффект: Инверсия WASD
│
├── InputSequencer.h/cpp     ← Утилита: ввод и буфер обмена
├── main.cpp                 ← Entry point приложения
└── ... (прочие системные файлы)
```

## 🔑 Ключевые файлы для изменения

### Если нужно изменить...

| Что?                      | Файл                                                                      |
| ------------------------- | ------------------------------------------------------------------------- |
| **Тайминг рулетки**       | `Config.h` → `ROULETTE_*_MS`                                              |
| **Длительность эффекта**  | `Config.h` → `EFFECT_DURATION_*_MS`                                       |
| **Команды консоли игры**  | `Config.h` → `SENSITIVITY_*_CMD`, `FPS_*_CMD`                             |
| **Логика эффектов**       | `EffectSystem.cpp` → `StartRandomEffect()`, `ActivateEffect()`            |
| **Визуальные параметры**  | `Config.h` → `BUTTON_*`, `MARGIN_*`, `ERASE_RADIUS_PX`                    |
| **Добавить новый эффект** | `Types.h` (enum), `Config.h` (duration), `EffectSystem.cpp` (switch case) |

## 🎯 Основные функции

### EffectSystem API (главный API)

```cpp
// Инициализация (один раз при старте)
void InitEffectSystem(HWND overlayHwnd);

// Управление системой
void StartSystem();           // Запустить цикл эффектов
void StopSystem();            // Остановить
void UpdateEffectSystem();    // Вызывать из message loop

// Получение информации
SystemState GetSystemState(); // Текущее состояние
EffectType GetCurrentEffect(); // Какой эффект активен
bool IsEffectAlive();         // Активен ли эффект прямо сейчас
DWORD GetEffectRemainingTime(); // Сколько осталось времени

// DEV API (для тестирования)
void Dev_ForceEffect(EffectType type);   // Активировать конкретный эффект
void Dev_ForceRoulette();                // Принудительно запустить рулетку
void Dev_StopEffect();                   // Остановить текущий эффект
void Dev_StartPreviewRoulette();         // Показать рулетку без эффекта
void Dev_StopPreviewRoulette();          // Скрыть превью рулетки
```

### Window API

```cpp
bool CreateMainWindow(HINSTANCE hInstance); // Создать окно оверлея
void RunMessageLoop();                      // Запустить цикл сообщений
HWND GetOverlayWindow();                    // Получить handle окна

// Вспомогательные функции для окна
void SetOverlayMousePassThrough(HWND hwnd, bool passThrough);
void SetOverlayColorkey(HWND hwnd);
void SetOverlayAlpha(HWND hwnd);
```

### ControlWindow API

```cpp
bool CreateControlWindow(HINSTANCE hInstance);
HWND GetControlWindow();
```

## 📊 State Machine (EffectSystem)

```
OFF
  ↓ StartSystem()
WAITING (45s - 5s = 40s)
  ↓ Автоматически при истечении времени
ROLLING (5s)
  ↓ Автоматически выбирает эффект
EFFECT (N секунд в зависимости от типа)
  ↓ По истечении длительности
WAITING (цикл повторяется)
```

## 💡 Типичный сценарий использования

```cpp
// В main.cpp
int main() {
    // 1. Инициализация
    InitEffectSystem(hOverlay);

    // 2. Запуск системы (запустит рулетку через 45 сек)
    StartSystem();

    // 3. Message loop обновляет EffectSystem
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        UpdateEffectSystem();  // ← Вызывается каждый frame
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
```

## 🔍 Как найти что-то

| Ищу...                | Где искать                                          |
| --------------------- | --------------------------------------------------- |
| Enum эффектов         | `Types.h` → `enum class EffectType`                 |
| Константу времени     | `Config.h` → `constexpr DWORD ...`                  |
| Логику выбора эффекта | `EffectSystem.cpp` → `StartRandomEffect()`          |
| Инициализацию эффекта | `EffectSystem.cpp` → `ActivateEffect()` switch      |
| Остановку эффекта     | `EffectSystem.cpp` → `StopInternalEffect()` switch  |
| Кнопки UI             | `ControlWindow.cpp` → `CreateButton()` calls        |
| Обработку кликов      | `ControlWindow.cpp` → `ControlWndProc()` WM_COMMAND |

## 🚀 Быстрый старт для добавления нового эффекта

1. **Создать файлы**

   ```cpp
   // NewEffect.h
   #pragma once
   void StartNewEffect();
   void StopNewEffect();
   ```

2. **Добавить в Types.h**

   ```cpp
   enum class EffectType {
       // ...
       NewEffect,  // ← Новый
   };
   ```

3. **Добавить в Config.h**

   ```cpp
   constexpr DWORD EFFECT_DURATION_NEW_EFFECT_MS = 20000;
   ```

4. **Добавить в EffectSystem.cpp**

   ```cpp
   #include "NewEffect.h"

   static DWORD GetEffectDuration(EffectType type) {
       // ...
       case EffectType::NewEffect: return EFFECT_DURATION_NEW_EFFECT_MS;
   }

   static void ActivateEffect(EffectType type) {
       // ...
       case EffectType::NewEffect: break;  // инициализация если нужна
   }

   static void StopInternalEffect() {
       // ...
       case EffectType::NewEffect: StopNewEffect(); break;
   }
   ```

5. **Добавить кнопку в ControlWindow.cpp**

   ```cpp
   enum ControlWindowButtonId {
       BTN_NEW_EFFECT = 1208,  // ← Новая
   };

   CreateButton(..., L"NEW EFFECT", BTN_NEW_EFFECT, ...);

   case BTN_NEW_EFFECT: g_pendingAction = PendingAction::ForceNewEffect; break;
   ```

## ⚡ Полезные горячие клавиши

При разработке в Visual Studio:

- **Ctrl+F5** - Запустить без отладки
- **F5** - Запустить с отладкой
- **Ctrl+Shift+B** - Собрать проект
- **View → Output** - Просмотр OutputDebugString()

## 📌 Важные замечания

1. **Все тайминги в миллисекундах**
2. **GetTickCount() может переполниться** - но за время работы приложения это не будет проблемой
3. **Оверлей прозрачен и невидим пока нет эффектов** - это нормально
4. **Некоторые эффекты захватывают ввод** (Erase, HalfScreenCover) - SetOverlayMousePassThrough учитывает это
5. **Консольные команды** должны соответствовать вашей игре
6. **Ctrl+Q или Alt+F4** обычно закрывает приложение
