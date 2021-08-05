# Overview
Transport catalog is a system of storing transport routes and processing related requests. It works in two modes:
- make_base - read **base requests**, **routing settings** and  **serialization settings**, preprocess optimal routes and related data, create database and serialize it to a file
- process_requests - read **stat requests** and **serialization settings**, deserialize database from the file and process requests to the database

The program performs input and output via standard input/output streams in JSON format.
# Content
 - [Input description](#Input&#32description)
 - [Output description](#Output&#32description)
 - [Restrictions](#Restrictions)
 - [Complexity](#Complexity)
 - [Example](#Example)
# Input description
The program expects a JSON dictionary to be input. Dictionary keys in JSON can be arranged in any order. Formatting (that is, whitespace characters around parentheses, commas, and colons) does not matter. The following is a description of the contents of the input JSON dictionary in the format: key, example, decryption.
## make_base mode
#### routing_settings
--------
```
"routing_settings": {
  "bus_wait_time": 6,
  "bus_velocity": 40.7725
}
```
A dictionary that sets the settings of bus routes (for all routes in the database). Keys:
 - *"bus_wait_time"* — a non-negative integer, the waiting time of the bus at the stop in minutes. At this stage, it is assumed that whenever a person comes to a stop and whatever this stop is, he will wait for any bus for exactly the specified number of minutes 
 - *"bus_velocity"* — a positive real number, the speed of the bus in km / h. It is assumed that the speed of any bus is constant and exactly equal to the specified number. The time of parking at stops is not taken into account, the time of acceleration and braking neither

#### serialization_settings
--------
```
"serialization_settings": {
        "file": "/tmp/tmp7f2a431a"
    }
```
A dictionary that sets serialization settings. Keys:
- *"file"* - a string, the name of the file to save the serialized database to
#### base_requests
--------
```
"base_requests": [
    {
      "type": "Bus",
      "name": "297",
      "stops": [
        "Hudson St at Vreeland Ave",
        "Hudson St at Kansas St"
      ],
      "is_roundtrip": true
    },
    {
      "type": "Stop",
      "road_distances": {
        "Hudson St 100' N of Lafayette St": 2600
        "Hudson St at Kansas St": 200
      },
      "longitude": 37.6517,
      "name": "Hudson St at Vreeland Ave",
      "latitude": 55.574371
    }
]
```
An array of an arbitrary number of stat requests. Each stat request is a dictionary containing the key  *"type"*, which corresponds to a string containing the stat request type. The content of the stat requests differs depending on the type
##### Types of base requests

#### Stop 
```
{
    "type": "Stop",
    "name": "Military Park Station",
    "latitude": 43.590317,
    "longitude": 39.746833,
    "road_distances": {
        "Newark Penn Station": 4300,
        "Newark, NJ Penn Station": 2000
    }
}
```
Request to add a stop to the database. Keys:
 - *"name"* - string, the name of the stop
 - *"latitude"* - a real number from -90 to 90, latitude of the stop in GPS-coordinates.
 - *"longitude"* - a real number from -180 to 180, longitude of the stop in GPS-coordinates.
 - *"road_distances"* - map of the path along the road from this stop to neighboring ones. Key - string, neighbor station name, value - a non-negative integer, distance in meters. By default, the distance from stopA to stopB is assumed to be equal to the distance from stopB to stopA, unless the distance from stopB to stopA is explicitly specified when adding stopB.
#### Bus
```
{
    "type": "Bus",
    "name": "635",
    "stops": [
        "Van Houten Ave at Althea S",
        "Passaic Ave at Ascension Pl",
        "Passaic Ave at Paulison Ave"
      ],
    "is_roundtrip": false
}
```
Request to add a bus route to the database. Keys:
- *"name"* - string, name of the bus
- *"stops"* - an array containing the lines - the names of the stops that the bus passes through in the order of its sequence
- *"is_roundtrip"* - a flag indicating whether the bus route is circular. The circular route starts and ends at one station. A non-circular route runs from the initial station to the final one, turns around and goes in the opposite direction. Let's take for example a bus with *"stops"* : ["A", "B", "C", "A"]. If *"is_roundtrip"* : true, then its route is: A-B-C-A. If *"is_roundtrip"* : false, its route is A-B-C-A-C-B-A. At the final stop, buses drop off all passengers and leave for the bus fleet. If a passenger wants to pass through the final station on a circular bus, he will have to get off at the final stop and wait for the same bus.

## process_requests mode
#### serialization_settings
--------
```
"serialization_settings": {
        "file": "/tmp/tmp7f2a431a"
    }
```
A dictionary that sets serialization settings. Keys:
- *"file"* - a string containing the name of the file from which you want to deserialize the database
####  stat_requests
--------
```
  "stat_requests": [
    {
      "type": "Stop",
      "name": "Main Ave at Washington",
      "id": 1
    },
    {
      "type": "Bus",
      "name": "828",
      "id": 2
    },
    {
      "type": "Route",
      "from": "Main Ave Terminal",
      "to": "Court St at Main St, County Court",
      "id": 3
    }
    {
      "type": "Stop",
      "name": "Court St at Main St, County Court",
      "id": 4
    },
]
```
An array of an arbitrary number of stat requests. Each stat request contains keys: 
 - *"id"* - целое неотрицательное число, уникальный номер запроса, который так же содержится в ответе на запрос под ключём *"request_id"*. Служит для сопоставления [stat_requests](####stat_requests) и [stat_requests repsnoses](#Output description) пользователем программы.
 - *"type"* - строка - тип stat запроса. 
##### Типы стат запросов
#### Stop
```
{
    "type": "Stop",
    "name": "Main Ave at Washington",
    "id": 1
}
```
Вывести информацию об остановке. Ссылкой на ответ к реквесту. Ключи:
 - *"name"* - строка, имя остановки

#### Bus
```
{
    "type": "Bus",
    "name": "828",
    "id": 2
}
```
Вывести информацию об автобусном маршруте. Ключи:
 - *"name"* - строка, имя автобусного маршрута

#### Route
```
{
    "type": "Route",
    "from": "Main Ave Terminal",
    "to": "Court St at Main St, County Court",
    "id": 3
}
```
Построить и вывести кратчайший маршрут между двумя остановками. Ключи:
 - *"from"* — строка, название остановки, в которой нужно начать маршрут.
 - *"to"* — строка, название остановки, в которой нужно закончить маршрут.

# Output description
В режиме **make_base** в случае успешного выполнения программа не имеет вывода. 
В режиме **process_requests** выводом являются ответы на стат запросы. Каждый ответ имеет ключ *"request_id"*, равный значению *"id"* из соответствующего [stat_request](####  stat_requests) стат запроса (см описание stat запросов). Ссылкой!

#### Ответ "not found"
--------
```
{
    "error_message": "not found",
    "request_id": 11
}
```
Выводится в случае, если в базе данных нет необходимой информации (нет запрашиваемого автобуса, остановки, либо искомый маршрут нельзя построить) Данный ответ универсален для любого типа стат запроса
#### Ответ на стат запрос типа [Stop](####Stop)
--------
```
{
    "buses": [
        "297",
        "635",
        "828"
    ],
    "request_id": 4
}
``` 
Ключи:
 - *"buses"* - массив строк, автобусные маршруты, проходящие через остановку. Может быть пустым, если через остановку не проходит ни один автобусный маршрут

#### Ответ на стат запрос типа [Bus](####Bus)
--------
```
{
    "stop_count": 11,
    "route_length": 15790,
    "unique_stop_count": 6,
    "curvature": 1.31245,
    "request_id": 3
}
```
Ключи:
 - *"route_length"* - неотрицательное вещественное число, длинна маршрута измерения по дорогам в метрах
 - *"stop_count"* - неотрицательное натуральное число, количество остановок, через которое проходит автобусный маршрут
 - *"unique_stop"* - неотрицательное натуральное число, количество уникальных остановок, через которые проходит автобусный маршрут
 - *"curvature"* - неотрицательное вещественное число, отношение кратчайшего пути между остановками маршрута на поверхности сферы (ортодромии) к пути, измеренного по дорогам. Может быть меньше единицы лишь в случае использования хордовых тоннелей. В остальных случаях больше либо равен единице

#### Ответ на стат запрос типа [Route](####Route)
--------
```
{
    "total_time": 11.235,
    "items": [
        {
            "time": 6,
            "type": "Wait",
            "stop_name": "Biryulyovo Zapadnoye"
        },
        {
            "span_count": 2,
            "bus": "297",
            "type": "Bus",
            "time": 5.235
        }
    ],
    "request_id": 4
}
```
Ключи:
 - *"total_time"* - неотрицательное вещественное число, минимальное время, необходимое что бы осуществить поездку, в минутах
 - *"items"* - массив элементов маршрута, каждый из которых описывает активность пассажира, требующую временных затрат

##### Типы элементов маршрута
#### Wait
```
{
    "time": 2,
    "stop_name": "Biryulyovo Tovarnaya",
    "type": "Wait"
}
```
Подождать автобуса на остановке. Ключи:
 - *"stop_name"* - строка, имя остановки
 - *"time"* - неотрицательное вещественное число, время, которое необходимо подождать на остановке в минутах

#### Bus
```
{
    "time": 1.78,
    "bus": "635",
    "span_count": 1,
    "type": "Bus"
}
```
Проехать на автобусе, начиная свою поездку с остановки, указанной в предыдущем элементе *"Wait"*. Ключи:
 - *"span_count"* натуральное число, колличество остановок, которые необходимо проехать
 - *"time"* неотрицательное вещественное число, время которое необходимо потратить на поездку в минутах
 - *"bus"* строка, имя автобусного маршрута

# Restrictions
Целые числа имеют диапазон минимум от –32768 до 32767, для большинства систем −2 147 483 648 до 2 147 483 647.
Вещественные числа имеют диапазон +/- 1.7E-308 до 1.7E+308. Ожидается, что вещественные числа не подаются на вход в экспоненциальной записи. Выводятся вещественные числа с шестью значащими цифрами.
Ожидается, что для всех заданных Bus заданы так же и Stops, содержащихся в Bus, а так же расстояния между всеми Stops, через которые следует Bus.
Ожидается, что остановка определена не более чем в одном base_request Stop.
# Сomplexity
#### Режим make_base
Основные затраты памяти и процессорного времени связаны с препроцессингом оптимальных путей, остальные затраты пренебрежимо малы. Так как программа реализует препроцессинг всех возможных путей с помощью алгоритма Дейкстры, её сложность составляет О(V^3 + E), где V - количество остановок в базе данных, а E - суммарное количество остановок во всех автобусных маршрутах, содержащихся в базе данных. Кроме того, ей требуется O(V^2) объём памяти.
#### Режим process_requests
Сложность обработки стат запросов в зависимости от типа запроса:
 - *"Stop"* - линейная, относительно количества автобусов, проходящих через эту остановку. 

 - *"Bus*" - константная.

 - *"Route"* - линейная, относительно элементов итогового маршрута.

# Example
Run transport catalog in make_base mode:
```
transport_catalog make_base
```
Put information about routing settings, serialization settings, stops and buses to standart input stream:
```
{
  "serialization_settings": {
    "file": "/tmp/transportcatalogdfs31235gsad324sct422/tmpw58tetx6"
  },
"routing_settings": {
    "bus_wait_time": 6,
    "bus_velocity": 40
  },
"base_requests": [
    {
      "type": "Bus",
      "name": "297",
      "stops": [
        "Biryulyovo Zapadnoye",
        "Biryulyovo Tovarnaya",
        "Universam",
        "Biryulyovo Zapadnoye"
      ],
      "is_roundtrip": true
    },
    {
      "type": "Bus",
      "name": "635",
      "stops": [
        "Biryulyovo Tovarnaya",
        "Universam",
        "Prazhskaya"
      ],
      "is_roundtrip": false
    },
    {
      "type": "Stop",
      "road_distances": {
        "Biryulyovo Tovarnaya": 2600
      },
      "longitude": 37.6517,
      "name": "Biryulyovo Zapadnoye",
      "latitude": 55.574371
    },
    {
      "type": "Stop",
      "road_distances": {
        "Prazhskaya": 4650,
        "Biryulyovo Tovarnaya": 1380,
        "Biryulyovo Zapadnoye": 2500
      },
      "longitude": 37.645687,
      "name": "Universam",
      "latitude": 55.587655
    },
    {
      "type": "Stop",
      "road_distances": {
        "Universam": 890
      },
      "longitude": 37.653656,
      "name": "Biryulyovo Tovarnaya",
      "latitude": 55.592028
    },
    {
      "type": "Stop",
      "road_distances": {},
      "longitude": 37.603938,
      "name": "Prazhskaya",
      "latitude": 55.611717
    }
  ]
}
```
After the program finished, run transport catalog in process requests mode:
```
transport_catalog process_requests
```
Put serialization settings and requests to the database to standart input stream:
```
{
  "serialization_settings": {
    "file": "/tmp/transportcatalogdfs31235gsad324sct422/tmpw58tetx6"
  },
  "stat_requests": [
    {
      "type": "Bus",
      "name": "297",
      "id": 1
    },
    {
      "type": "Bus",
      "name": "635",
      "id": 2
    },
    {
      "type": "Stop",
      "name": "Universam",
      "id": 3
    },
    {
      "type": "Route",
      "from": "Biryulyovo Zapadnoye",
      "to": "Universam",
      "id": 4
    },
    {
      "type": "Route",
      "from": "Biryulyovo Zapadnoye",
      "to": "Prazhskaya",
      "id": 5
    }
  ]
}
```
Optput of transport catalog:
```
[
    {
        "curvature": 1.42963,
        "unique_stop_count": 3,
        "stop_count": 4,
        "request_id": 1,
        "route_length": 5990
    },
    {
        "curvature": 1.30156,
        "unique_stop_count": 3,
        "stop_count": 5,
        "request_id": 2,
        "route_length": 11570
    },
    {
        "request_id": 3,
        "buses": [
            "297",
            "635"
        ]
    },
    {
        "total_time": 11.235,
        "items": [
            {
                "time": 6,
                "type": "Wait",
                "stop_name": "Biryulyovo Zapadnoye"
            },
            {
                "span_count": 2,
                "bus": "297",
                "type": "Bus",
                "time": 5.235
            }
        ],
        "request_id": 4
    },
    {
        "total_time": 24.21,
        "items": [
            {
                "time": 6,
                "type": "Wait",
                "stop_name": "Biryulyovo Zapadnoye"
            },
            {
                "span_count": 2,
                "bus": "297",
                "type": "Bus",
                "time": 5.235
            },
            {
                "time": 6,
                "type": "Wait",
                "stop_name": "Universam"
            },
            {
                "span_count": 1,
                "bus": "635",
                "type": "Bus",
                "time": 6.975
            }
        ],
        "request_id": 5
    }
]
```
