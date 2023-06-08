import time
import socket
import select
import json
import plotly.graph_objects as go
import matplotlib.pyplot as plt
import numpy as np


HOST = 'localhost'
PORT = 8888

def ReadFromServer(socket, length):
    data = b''
    total_length = 0
    while total_length < length:
        chunk = socket.recv(length - total_length)
        data += chunk
        total_length += len(chunk)
    return data


class Client:
    def __del__(self):
        self.client_socket.close()
        return

    def initClient(self, port):
        self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        return

    def connectToServer(self, port):
        self.client_socket.connect((HOST, PORT))
        return

    def run(self, port):
        self.initClient(port)
        self.connectToServer(port)

        response_json = self.receiveMessage()
        print('Message from server:', response_json)

        return

    def receiveMessage(self):
        len = int(ReadFromServer(self.client_socket, 6).decode())
        response = ReadFromServer(self.client_socket, len).decode()
        response_json = json.loads(response, strict=False)
        return response_json

    def sendMessage(self, message):
        self.client_socket.send(self.parseLength(message).encode())
        self.client_socket.send(message.encode())

    def parseLength(self, message):
        message_length = len(message)
        message_length_str = str(message_length)
        while len(message_length_str) < 6:
            message_length_str = "0" + message_length_str
        return message_length_str


    def hasAnyMessages(self):
        ready_to_read, _, _ = select.select([self.client_socket], [], [], 1)
        if self.client_socket in ready_to_read:
            return True
        else:
            return False


def ReceiveMessage(client):
    message = client.receiveMessage()
    response = message["response"]
    return response


def SendMessage(client, message):
    client.sendMessage(message)
    print("Message is successfully sent");


def GetUserInput():
    print("Print some message or # to stop chatting:")
    request_type = input()

    if not request_type:
        return None
    if request_type == "#":
        return request_type

    request_body = {}
    request_body["request-type"] = request_type

    if request_type == "ADD_RECORD":
        print("Print new record data:")
        record_data = input()
        request_body["data"] = record_data
    elif request_type == "REMOVE_RECORD":
        print("Print record data to remove:")
        record_data = input()
        request_body["data"] = record_data
    elif request_type == "GET_DIAGRAM":
        print("Print diagram type:")
        diagram_type = input()
        print("Print `yes` in case if you want to sort the results, and `no` otherwise:")
        should_sort_input = input()
        should_sort = "no"
        if should_sort_input == "yes":
            should_sort = "yes"
        request_body["diagram-type"] = diagram_type
        request_body["sort"] = should_sort
    elif request_type == "GET_SLICE":
        slice = []
        while True:
            print("Print dimension name:")
            dimension_name = input()
            if not dimension_name:
                break
            print("Print value to slice:")
            value = input()
            slice.append({"dimension": dimension_name, "value": value})
        request_body["slice"] = slice
        print("Print `yes` in case if you want to sort the results, and `no` otherwise:")
        should_sort_input = input()
        should_sort = "no"
        if should_sort_input == "yes":
            should_sort = "yes"
        request_body["sort"] = should_sort

    message = {"request": request_body}

    return message


def VisualizeTotalPricePerDataDiagram(response):
    cube = response["cube"]

    keys = []
    values = []
    for entry in cube:
        key = entry["keys"][0]
        value = entry["value"]
        keys.append(key)
        values.append(int(value))

    fig = go.Figure(data=go.Scatter(
        x=keys,
        y=values,
    ))
    fig.update_layout(
        title='Total price per data',
        xaxis=dict(title='Data'),
        yaxis=dict(title='Price')
    )

    fig.show()


def VisualizeProductsCountDiagram(response):
    cube = response["cube"]

    keys = []
    values = []
    for entry in cube:
        key = entry["keys"][0]
        value = entry["value"]
        keys.append(key)
        values.append(int(value))

    fig = go.Figure(data=[go.Pie(labels=keys, values=values)])

    fig.update_traces(hoverinfo='label+percent', textinfo='value', textfont_size=12,
                      marker=dict(line=dict(color='#000000', width=1)))
    fig.show()


def VisualizeDeliveriesPerDataAndTime(response):
    keys_x = sorted(set(item["keys"][0] for item in response["cube"]))
    keys_y = sorted(set(item["keys"][1] for item in response["cube"]))
    matrix = np.full((len(keys_y), len(keys_x)), np.nan)
    for item in response["cube"]:
        x_index = keys_x.index(item["keys"][0])
        y_index = keys_y.index(item["keys"][1])
        matrix[y_index, x_index] = int(item["value"])
    sorted_indices_x = np.argsort(keys_x)
    sorted_indices_y = np.argsort(keys_y)
    sorted_matrix = matrix[sorted_indices_y][:, sorted_indices_x]
    sorted_x_values = np.array(keys_x)[sorted_indices_x]
    sorted_y_values = np.array(keys_y)[sorted_indices_y]
    fig = go.Figure(data=go.Heatmap(
        z=sorted_matrix,
        x=sorted_x_values,
        y=sorted_y_values,
        colorscale='Viridis'
    ))
    fig.update_layout(
        title='Heatmap',
        xaxis=dict(title='Data'),
        yaxis=dict(title='Time')
    )
    fig.show()


def VisualizeDeliveriesPerDistanceAndTime(response):
    cube = response["cube"]

    x_values = sorted(set(float(item['keys'][0]) for item in response["cube"]))
    y_values = sorted(set(item['keys'][1] for item in response["cube"]))

    x_coords = []
    y_coords = []
    values = []
    for entry in cube:
        keys = entry["keys"]
        value = entry["value"]
        x_coord = x_values.index(float(keys[0]))  # Используем индекс значения из x_values
        y_coord = y_values.index(keys[1])  # Используем индекс значения из y_values
        x_coords.append(x_coord)
        y_coords.append(y_coord)
        values.append(20 * int(value))

    fig = go.Figure(data=go.Scatter(
        x=x_coords,
        y=y_coords,
        mode='markers',
        marker=dict(
            size=values,
        )
    ))

    fig.update_layout(
        title='Scatter Plot: Deliveries per distance and time',
        xaxis=dict(
            title='Distance',
            tickvals=list(range(len(x_values))),
            ticktext=x_values
        ),
        yaxis=dict(
            title='Time',
            tickvals=list(range(len(y_values))),
            ticktext=y_values
        )
    )
    fig.show()


def VisualizePriceRangePerData(response):
    cube = response["cube"]

    dates = []
    min_prices = []
    max_prices = []
    for entry in cube:
        data = entry["keys"][0]
        data
        value = json.loads(entry["value"])
        dates.append(data)
        min_prices.append(int(value[0]))
        max_prices.append(int(value[1]))

    fig = go.Figure()

    fig.add_trace(go.Scatter(
        x=dates,
        y=[(min_price + max_price) / 2 for min_price, max_price in zip(min_prices, max_prices)],
        error_y=dict(
            type='data',
            array=[(max_price - min_price) / 2 for min_price, max_price in zip(min_prices, max_prices)],
            visible=True
        ),
        mode='markers',
        marker=dict(color='blue', size=10),
        name='Average price'
    ))

    fig.update_layout(
        title='Price Range per data',
        xaxis=dict(title='Data'),
        yaxis=dict(title='Price')
    )

    fig.show()


def VisualizeCouriersRevenue(response):
    cube = response["cube"]

    courier_ids = []
    revenues = []
    for entry in cube:
        courier_id = entry["keys"][0]
        revenue = int(entry["value"])
        courier_ids.append(courier_id)
        revenues.append(revenue)

    fig = go.Figure(data=go.Bar(x=courier_ids, y=revenues))
    fig.update_layout(title='Couriers revenue',
                      xaxis_title='Courier ID',
                      yaxis_title='Total revenue')
    fig.show()


def PrintRecordsPerData(response):
    cube = response["cube"]
    for entry in cube:
        keys = entry["keys"]
        value = entry["value"]
        print(keys)
        print(value)


def ProcessDiagramResults(request, client):
    while not client.hasAnyMessages():
        continue
    response_str = ReceiveMessage(client)
    response_json = json.loads(response_str, strict=False)

    diagram_type = request["request"]["diagram-type"]

    if diagram_type == "TOTAL_PRICE_PER_DATA":
        VisualizeTotalPricePerDataDiagram(response_json)
    elif diagram_type == "PRODUCTS_COUNT":
        VisualizeProductsCountDiagram(response_json)
    elif diagram_type == "DELIVERIES_PER_DATA_AND_TIME":
        VisualizeDeliveriesPerDataAndTime(response_json)
    elif diagram_type == "DELIVERIES_PER_DISTANCE_AND_TIME":
        VisualizeDeliveriesPerDistanceAndTime(response_json)
    elif diagram_type == "PRICE_RANGE_PER_DATA":
        VisualizePriceRangePerData(response_json)
    elif diagram_type == "COURIERS_REVENUE":
        VisualizeCouriersRevenue(response_json)
    elif diagram_type == "RECORDS_PER_DATA":
        PrintRecordsPerData(response_json)


def PrintSlice(response):
    cube = response["cube"]
    for entry in cube:
        keys = entry["keys"]
        value = entry["value"]
        print(keys)
        # print(value)


def ProcessRequestResults(request, client):
    request_type = request["request"]["request-type"]

    if request_type == "GET_RECORDS":
        while not client.hasAnyMessages():
            continue
        response_str = ReceiveMessage(client)
        response_json = json.loads(response_str, strict=False)
        for record in response_json:
            print(record)
    elif request_type == "GET_DIAGRAM":
        ProcessDiagramResults(request, client)
    elif request_type == "GET_SLICE":
        while not client.hasAnyMessages():
            continue
        response_str = ReceiveMessage(client)
        response_json = json.loads(response_str, strict=False)
        PrintSlice(response_json)


def StartChatting(client):
    while True:
        while client.hasAnyMessages():
            print("Server has some messages")
            message = ReceiveMessage(client)
            print("New message from server: {}".format(message))

        request = GetUserInput()

        if not request:
            continue
        if request == "#":
            break

        request_json = json.dumps(request)
        print("New request: {}".format(request_json))
        SendMessage(client, request_json)

        ProcessRequestResults(request, client)


if __name__ == '__main__':
    client = Client()

    client.run(PORT)
    StartChatting(client)
