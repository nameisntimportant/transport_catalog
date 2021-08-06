# Overview
Transport catalog is a system of storing transport routes and processing related requests. It works in two modes:
- make_base - read **base requests**, **routing settings** and  **serialization settings**, preprocess optimal routes and related data, create database and serialize it to a file
- process_requests - read **stat requests** and **serialization settings**, deserialize database from the file and process requests to the database

The program performs input and output via standard input/output streams in JSON format.
# Content
 - [Input](#Input)
 - [Output](#Output)
 - [Restrictions](#Restrictions)
 - [Complexity](#Complexity)
 - [Example](#Example)
# Input
The program expects a JSON dictionary to be input. Dictionary keys in JSON can be arranged in any order. Formatting (that is, whitespace characters around parentheses, commas, and colons) does not matter. The following is a description of the contents of the input JSON dictionary in the format: key, example, decryption
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
 - *"bus_wait_time"* — a nonnegative integer, the waiting time of the bus at the stop in minutes. At this stage, it is assumed that whenever a person comes to a stop and whatever this stop is, he or she will wait for any bus for exactly the specified number of minutes 
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
 - *"latitude"* - a real number from -90 to 90, latitude of the stop in GPS-coordinates
 - *"longitude"* - a real number from -180 to 180, longitude of the stop in GPS-coordinates
 - *"road_distances"* - map of the path along the road from this stop to neighboring ones. Key - string, neighbor station name, value - a nonnegative integer, distance in meters. By default, the distance from stopA to stopB is assumed to be equal to the distance from stopB to stopA, unless the distance from stopB to stopA is explicitly specified when adding stopB
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
- *"stops"* - an array of strings - the names of the stops that the bus passes through in the order of its sequence
- *"is_roundtrip"* - a flag indicating whether the bus route is circular. The circular route starts and ends at one station. A non-circular route runs from the initial station to the final one, turns around and goes in the opposite direction. Let's take for example a bus with *"stops"* : ["A", "B", "C", "A"]. If *"is_roundtrip"* : true, then its route is: A-B-C-A. If *"is_roundtrip"* : false, its route is A-B-C-A-C-B-A. At the final stop, buses drop off all passengers and leave for the bus fleet. If a passenger wants to pass through the final station on a circular bus, he or she will have to get off at the final stop and wait for the same bus

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
    }
]
```
An array of an arbitrary number of stat requests. Each stat request contains keys: 
 - *"id"* - a nonnegative integer, a unique request number, which is also contained in the response to the request under the key *"request_id"*. It is used for matching stat_requests and stat_requests responses by the program user
 - *"type"* - string, type of stat request
##### Types of stat requests
#### Stop
```
{
    "type": "Stop",
    "name": "Main Ave at Washington",
    "id": 1
}
```
Output information about the stop to standard output. Keys:
 - *"name"* - string, the name of the stop

#### Bus
```
{
    "type": "Bus",
    "name": "828",
    "id": 2
}
```
Output information about the bus route to standard output. Keys:
 - *"name"* - string, the name of the bus route

#### Route
```
{
    "type": "Route",
    "from": "Main Ave Terminal",
    "to": "Court St at Main St, County Court",
    "id": 3
}
```
Build and output the shortest route between two stops. Keys:
 - *"from"* — string, the name of the stop where you want to start the route
 - *"to"* — string, the name of the stop where you want to stop the route

A passenger can change between bus routes during the journey, but he or she can not walk between stops
# Output
In the **make_base** mode, if the program is executed successfully, it has no output

In the **process_requests** mode, the output is responses to stat requests. Each response has a key *"request_id"* equal to the value under the key *"id"* from the corresponding stat request

#### Response "not found"
--------
```
{
    "error_message": "not found",
    "request_id": 11
}
```
Output if the requested information is not in the database (there is no requested bus, stop, or the desired route cannot be built). This response is universal for any type of stat request
#### Response to a [Stop](####Stop) type stat request
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
Keys:
- *"buses"* - array of strings, bus routes passing through the stop. It can be empty if no bus route passes through the stop

#### Response to a [Bus](####Bus) type stat request
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
Keys:
- *"route_length"* is a nonnegative integer, route length measuring on roads in meters
- *"stop_count"* is a nonnegative integer, number of stops, through which runs a bus route
- *"unique_stop"* is a nonnegative integer, the number of unique stops, through which the bus route
- *"curvature"* is a nonnegative real number, the ratio of the shortest path between the stops of the route on the surface of the sphere (orthodromy) to the path measured by roads. It can be less than one only if chord tunnels are used. In all other cases, it is greater than or equal to one

#### Response to a [Route](####Route) type stat request
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
Keys:
- *"total_time"* - a nonnegative real number, the minimum time required to make a trip, in minutes
- *"items"* - an array of route elements, each of which describes the passenger's activity that requires time

##### Types of route elements
#### Wait
```
{
    "time": 2,
    "stop_name": "Biryulyovo Tovarnaya",
    "type": "Wait"
}
```
Wait for the bus at the bus stop. Keys:
- *"stop_name"* - string, the name of the stop
- *"time"* - a nonnegative real number, the time to wait at the stop in minutes

#### Bus
```
{
    "time": 1.78,
    "bus": "635",
    "span_count": 1,
    "type": "Bus"
}
```
Take the bus, starting your trip from the stop specified in the previous element *"Wait"*. Keys:
- *"spin_count"* natural number, the number of stops that need to be traveled
- *"time"* nonnegative real number, the time it takes to overcome the path between stops in minutes
- *"bus"* string, the name of the bus route


# Restrictions
Integers have a minimum range from -32768 to 32767, for most systems -2147,483,648 to 2,147,483,647.

Real numbers have a range of +/- 1.7 E-308 to 1.7 E+308. It is expected that the real numbers are not under the numbers on the input in the exponential notation. Real numbers are output with six significant digits

It is expected that all stops contained in the bus routes are set in some Stop base request, as well as the distances between all the stops that the bus passes through
# Сomplexity
#### make_base mode
The main costs of memory and processor time are associated with the preprocessing of optimal paths, the remaining costs are negligible. Since the program implements preprocessing of all possible paths using Dijkstra's algorithm, its complexity is O (V^3 + E), where V is the number of stops in the database, and E is the total number of stops in all bus routes contained in the database. Memory usage asymptotics is O(V^2 + E).
#### process_requests mode
The complexity of processing stat requests, depending on the type of request:
- *"Stop"* - linear in the number of buses passing through this stop
- *"Bus*" - constant
- *"Route"* - linear in the number of elements of the final route


# Example
Run transport catalog in make_base mode:
```
transport_catalog make_base
```
Put information about routing settings, serialization settings, stops, and buses to standard input stream:
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
Put serialization settings and requests to the database to standard input stream:
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
Output of transport catalog:
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
