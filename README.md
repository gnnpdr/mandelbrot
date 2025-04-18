## Отображение множества Мандельброта с использованием различных оптимизаций

# Исследуем влияние методов ручной оптимизации на скорость отрисовки двумерного изображенияю

Программа отрисовывает изображение множества Мандельброта при вызове с командной `--draw-graph` из командной строки. При вызове также задается метод отрисовки цифрами от `0` до `2` соответственно приведенным ниже методам.  

Программа разными способами проходится по каждому пикселю изображения, цвет устанавливается с помощью функции, он зависит от количесва шагов, которые нужно сделать, чтобы выйти на бесконечное расстояние. В данном случае псевдобесконечность - расстояние в 10. Если точка не вышла за этот предел за определенное максимальное количество шагов, она принадлежит множеству, окрашивется в базовый цвет. В левом верхнем углу отображается FPS.

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

# Методы отрисовки

Проверим каждый метод отрисовки, сравнив среднее время выполнения трех запусков с флагами -O0 и -O3.
Для измерения времени будем использовать ассемблерную функцию rdtsc, получим данные в тиках процессора. Для измерения времени запускаем программу без флага `--draw-graph`, тогда будут провереды пять измерений. Сделаем три замера для каждого случая. Посчитаем среднее, погрешность среднего и погрешность отдельных измерений.

Используем формулы:  


$\sigma_{отд} = \sqrt{\dfrac{1}{n}\sum_{i=1}^{n}(x_i - x_{ср})^2}$

$\sigma_{ср} = \dfrac{\sigma_{отд}}{\sqrt{n}}$

---

Простейший

С помощью двойного цикла программа проходится по каждому пикселю изображения.

| flag     | t1, tick    |  t2, tick   | t3, tick    | $\overline{t}$, tick   |
|:--------:|:-----------:|:-----------:|:-----------:|:-----------:|
| -O0      | 2742020051  |2826440523   |2796925589   |2788462054   |
| -O3      | 1334881968  |1335572762   |1312357759   |1327604163   |

|  flag    | $\sigma_{отд}$, tick    |  $\sigma_{ср}$, tick    | $\epsilon_{отд}$, %    |  $\epsilon_{ср}$, %    |
|:--------:|:---------------------:|:---------------------:|:--------------------------:|:----------------------:|
| -O0      | 15643647              |6996051                |4,86                        |0,43                    |
| -O3      | 4822985               |2156904                |3,15                        |0,28                    |

Получается, компилятор способен ускорить выполнение программы в 2,13 раз без ручной оптимизации.

---

Массивы

Используем развертку цикла, совершаем одни и те же операции с массивами по 8 элементов, намекая компилятору с флагом -O3, что функции можно преобразовать в intrinsic, тем самым ускорив исполнение.

| flag     | t1, tick    |  t2, tick   | t3, tick    | $\overline{t}$, tick   |
|:--------:|:-----------:|:-----------:|:-----------:|:-----------:|
| -O0      | 4365738051  |4301069990   |4332930619   |4333246220   |
| -O3      | 792510492   |805860572    |803860861    |800743975    |

|  flag    | $\sigma_{отд}$, tick    |  $\sigma_{ср}$, tick    | $\epsilon_{отд}$, %    |  $\epsilon_{ср}$, %    |
|:--------:|:---------------------:|:---------------------:|:-----------------------:|:---------------------:|
| -O0      | 11807140              |5280314                |2,36%                    |0,21%                  |
| -O3      | 2629129               |1175782                |2,84%                    |0,25%                  |

Без флага -O3 компилятор не замечает, что можно использовать SIMD инструкции, время выполнения заметно выше. Разница в 5,41 раза, а ожидалось примерно в 8, так как обратывается одновременно по 8 значений. Вероятно, это связано с тем, что другая часть программы, не связанная с обработкой изображения, требует большого времени. С ипользование флага -O0 программа становится медленнее по сравнению с первой версией в 1,55 раз. Возможно, это связано с тем, что при каждой итерации основного цикла сокращены в 8 раз, но при кадой итерации она должна пройтись по нескольким циклам, чтобы обработать массивы чисел, которые программа при таком флаге не преобразовала в векторные данные.

---

SIMD

Один из методов оптимизации - использование структур SIMD (Single Instruction Multiple Data), мой ноутбук поддерживает AVX, векторы в 256 бит, способные хранить 8 значений типа float, этим мы и будем пользоваться. Такие инструкции позволяют исполнять операцию одновременно с различными значениями, меняется граница пропускной способности. С веторными данными работают intrinsic функции. Для их подключения используется библиотека immintrin.h и флаг при компиляции `-mavx2`. Стоит сказать, что не все устройства поддерживают векторные инструкции, переносимость на определенной машине можно проверить командой `__builtin_cpu_supperts("*")`, которая возвращает 0, если тип данных * не поддерживается. Подключаем библиотеку, используем intrinsic функции, их описание можно посмотреть на сайте [intel intrinsic guide](https://www.intel.com/content/www/us/en/support/ru-banner-inside.html "вот блин, что делать, не работает").

| flag     | t1, tick    |  t2, tick   | t3, tick    | $\overline{t}$, tick   |
|:--------:|:-----------:|:-----------:|:-----------:|:-----------:|
| -O0      | 1304991836  |1339500564   |1289256779   |1311249726   |
| -O3      | 311838672   |290258066    |305759628    |302618789    |

|  flag    | $\sigma_{отд}$, tick    |  $\sigma_{ср}$, tick    | $\epsilon_{отд}$, %    |  $\epsilon_{ср}$, %    |
|:--------:|:---------------------:|:---------------------:|:-----------------------:|:---------------------:|
| -O0      | 16253989              |7269005                |6,20%                    |0,55%                  |
| -O3      | 7037877               |3147434                |11,63%                   |1,04%                  |

Программа ускорилась в 4,33 раз при смене флага и в 4,39 по сравнению с первой версией при флаге -O3. Все это за счет уменьшения количества тактов на итерацию. Однако ускорение опять же не в 8 раз, как ожидалось, наверняка, по тем же причинам, что во втором случае.

# Выводы

Получается:
- Выбор флага при компиляции значительно влияет на скорость исполнения программы. 
- Ручные методы оптимизации токже могут помочь. 
- Векторые инструкции весьма эффективны.
