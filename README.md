# Отображение множества Мандельброта с использованием различных оптимизаций

Исследуем влияние методов ручной оптимизации на скорость отрисовки двумерного изображения.

Программа отрисовывает изображение множества Мандельброта при вызове с командной `-graph` из терминала. При этом также задается метод отрисовки цифрами от 0 до 2 соответственно приведенным ниже методам.

В левом верхнем углу отображаются FPS.

Программа разными способами проходится по каждому пикселю изображения, цвет устанавливается с помощью функции, он зависит от количесва шагов, которые нужно сделать, чтобы выйти на бесконечное расстояние. В данном случае псевдобесконечность - расстояние в 10. Если точка не вышла за этот предел за максимальное количество шагов - 200, она принадлежит множеству, окрашивается в базовый цвет.

Пример изображения:
![image](https://github.com/gnnpdr/mandelbrot/raw/main/images/mandelbrot.png)

На изображение можно влиять клавишами:

| Клавиша     | Действие         |
|:-----------:|:----------------:|
| up          | Смещение вверх   |
| down        | Смещение вниз    |
| right       | Смещение вправо  |
| left        | Смещение влево   |
| +           | Увеличение       |
| -           | Уменьшение       |

## Методы отрисовки

Проверим каждый метод отрисовки, сравнив среднее время выполнения по трем запускам с флагами -O0 и -O3.
Для измерения времени будем использовать класс `sf::Time` из библиотеки SFML, получаем значения в секундах.

---

### Простейший

С помощью двойного цикла программа проходится по каждому пикселю изображения.

| flag     | t1, sec    |  t2, sec   | t3, sec    | t_mid, sec   |
|:--------:|:----------:|:----------:|:----------:|:------------:|
| -O0      | 0,185088   |0,181537    |  0,190515  |0,185713      |
| -O3      | 0,117584   |0,115014    | 0,098183   |0,110260      | 

Получается, компилятор способен ускорить выполнение программы в 1,68 раз без ручной оптимизации.

---

### Массивы

Используем развертку цикла, совершаем одни и те же операции с массивами по 8 элементов, намекая компилятору, что функции можно преобразовать в intrinsic, тем самым ускорив исполнение.

| flag     | t1, sec    |  t2, sec   | t3, sec    | t_mid, sec   |
|:--------:|:----------:|:----------:|:----------:|:------------:|
| -O0      | 0,238554   |0,257941    |  0,264924  |0,253806      |
| -O3      | 0,077058   |0,078205    | 0,044298   |0,066520      |

Без флага -O3 компилятор не замечает, что можно использовать SIMD инструкции, время выполнения заметно выше. Разница в 3,82 раза, а ожидалось примерно в 8, так как обратывается одновременно по 8 значений. Вероятно, это связано с тем, что другая часть программы, не связанная с обработкой изображения, требует большого времени. С ипользованием флага -O0 программа становится медленнее по сравнению с первой версией в 1,37 раз. Возможно, это связано с тем, что при том, что итерации основного цикла сокращены в 8 раз, в каждом круге программа должна пройтись по нескольким циклам - обработать массивы чисел, которые программа при таком флаге не преобразовала в векторные данные.

---

### SIMD

Один из методов оптимизации - использование структур SIMD (Single Instruction, Multiple Data), мой ноутбук поддерживает AVX (Advanced Vector Extensions), векторы размером 256 бит, способные хранить 8 значений типа float, этим мы и будем пользоваться. Такие инструкции позволяют выполнять операцию одновременно с различными значениями, меняется граница пропускной способности.

С веторными данными работают intrinsic функции. Для их подключения используется библиотека immintrin.h и флаг при компиляции `-mavx2`. 

Стоит сказать, что не все устройства поддерживают векторные инструкции, переносимость на определенной машине можно проверить командой `__builtin_cpu_supperts("*")`, которая возвращает 0, если тип данных * не поддерживается.

Подключаем библиотеку, используем intrinsic функции, их описание можно посмотреть на сайте intel intrinsic guide.

| flag     | t1, sec    |  t2, sec   | t3, sec    | t_mid, sec   |
|:--------:|:----------:|:----------:|:----------:|:------------:|
| -O0      | 0,140136   |0,127509    |  0,097925  |0,121857      |
| -O3      | 0,027296   |0,028510    | 0,024393   |0,026733      |

Программа ускорилась в 4,56 раз при смене флага и в 4,12 по сравнению с первой версией при флаге -O3. Все это за счет уменьшения количества тактов на итерацию. Однако ускорение опять же не в 8 раз, как ожидалось, наверняка, по тем же причинам, что во втором случае.

# Выводы

Получается:
- Выбор флага при компиляции значительно влияет на скорость исполнения программы. 
- Ручные методы оптимизации также могут помочь. 
- Векторые инструкции весьма эффективны.
