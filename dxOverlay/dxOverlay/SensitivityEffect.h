#pragma once

/**
 * @file SensitivityEffect.h
 * @brief Эффект отключения чувствительности мыши
 *
 * Отключает контроль мыши установкой sensitivity = 0.0 в консоли игры
 */

/**
 * @brief Запустить эффект (отключить чувствительность)
 *
 * Эффект:
 * - Открывает консоль игры
 * - Выполняет команду: sensitivity 0.0
 * - Закрывает консоль
 *
 * @see StartSensitivityEffect()
 */
void StartSensitivityEffect();

/**
 * @brief Остановить эффект (восстановить чувствительность)
 *
 * Эффект:
 * - Открывает консоль игры
 * - Выполняет команду: sensitivity 0.2 (оригинальное значение)
 * - Закрывает консоль
 *
 * @see StopSensitivityEffect()
 */
void StopSensitivityEffect();
