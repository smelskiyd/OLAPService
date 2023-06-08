import tkinter as tk
import socket
import select
import json
from tkinter import messagebox
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

def reload_func():
    request_body = {}
    request_body["request-type"] = "RELOAD"
    message = {"request": request_body}
    request_json = json.dumps(message)
    SendMessage(client, request_json)
    messagebox.showinfo("Reload", "RELOAD: SUCCESS")

def save_func():
    request_body = {}
    request_body["request-type"] = "SAVE"
    message = {"request": request_body}
    request_json = json.dumps(message)
    SendMessage(client, request_json)
    messagebox.showinfo("Reload", "SAVE: SUCCESS")

def add_record_func():
    def submit():
        record_data = entry.get()
        messagebox.showinfo("Add Record", f"You entered: {record_data}")

        request_body = {}
        request_body["request-type"] = "ADD_RECORD"
        request_body["data"] = record_data
        message = {"request": request_body}
        request_json = json.dumps(message)
        SendMessage(client, request_json)
        messagebox.showinfo("Reload", "ADD_RECORD: SUCCESS")

        sub_window.destroy()

    sub_window = tk.Toplevel(root)
    sub_window.title("Add Record")

    label = tk.Label(sub_window, text="Enter record data:")
    label.pack()

    entry = tk.Entry(sub_window)
    entry.pack()

    submit_button = tk.Button(sub_window, text="Submit", command=submit)
    submit_button.pack()

def remove_record_func():
    def submit():
        record_data = entry.get()
        messagebox.showinfo("Remove Record", f"You entered: {record_data}")

        request_body = {}
        request_body["request-type"] = "REMOVE_RECORD"
        request_body["data"] = record_data
        message = {"request": request_body}
        request_json = json.dumps(message)
        SendMessage(client, request_json)
        messagebox.showinfo("Reload", "REMOVE_RECORD: SUCCESS")

        sub_window.destroy()

    sub_window = tk.Toplevel(root)
    sub_window.title("Remove Record")

    label = tk.Label(sub_window, text="Enter text:")
    label.pack()

    entry = tk.Entry(sub_window)
    entry.pack()

    submit_button = tk.Button(sub_window, text="Submit", command=submit)
    submit_button.pack()

def get_records_func():
    sub_window = tk.Toplevel(root)
    sub_window.title("Get Records")
    sub_window.geometry("800x600")

    request_body = {}
    request_body["request-type"] = "GET_RECORDS"
    message = {"request": request_body}
    request_json = json.dumps(message)
    SendMessage(client, request_json)

    while not client.hasAnyMessages():
        continue
    response_str = ReceiveMessage(client)
    response_json = json.loads(response_str, strict=False)

    listbox = tk.Listbox(sub_window)
    listbox.configure(width=800, height=600)
    listbox.pack()
    for record in response_json:
        listbox.insert(tk.END, record)

def VisualizeSlice(response):
    cube = response["cube"]

    sub_window = tk.Toplevel(root)
    sub_window.title("Slice")
    sub_window.geometry("800x600")

    listbox = tk.Listbox(sub_window)
    listbox.configure(width=800, height=600)
    listbox.pack()

    values = []
    for entry in cube:
        keys = entry["keys"]
        current_value = entry["value"]
        values.append(current_value)
        listbox.insert(tk.END, keys)

    def open_new_window(index):
        sub_window = tk.Toplevel(root)
        sub_window.title("Selected Item")
        sub_window.geometry("800x600")

        entry_value = json.loads(values[index])
        entry_list_box = tk.Listbox(sub_window)
        entry_list_box.configure(width=800, height=600)
        entry_list_box.pack()

        for value in entry_value:
            entry_list_box.insert(tk.END, json.dumps(value))

    def on_item_selected(event):
        selected_index = listbox.curselection()[0]
        open_new_window(selected_index)

    listbox.bind("<<ListboxSelect>>", on_item_selected)

def get_slice_func():
    slice = []

    def add_pair():
        pair_window = tk.Toplevel(root)
        pair_window.title("Add Pair")

        dimension_label = tk.Label(pair_window, text="Dimension name:")
        dimension_label.grid(row=0, column=0)
        dimension_entry = tk.Entry(pair_window)
        dimension_entry.grid(row=0, column=1)

        value_label = tk.Label(pair_window, text="Value:")
        value_label.grid(row=1, column=0)
        value_entry = tk.Entry(pair_window)
        value_entry.grid(row=1, column=1)

        def confirm_pair():
            x = dimension_entry.get()
            y = value_entry.get()
            print("Added pair:", x, y)
            slice.append({"dimension": x, "value": y})
            listbox.insert(tk.END, x + " " + y)
            pair_window.destroy()

        confirm_button = tk.Button(pair_window, text="Confirm", command=confirm_pair)
        confirm_button.grid(row=2, column=0, columnspan=2)

    def submit():
        should_sort = sort_var.get()
        should_sort_str = "no"
        if should_sort:
            should_sort_str = "yes"

        request_body = {}
        request_body["request-type"] = "GET_SLICE"
        request_body["slice"] = slice
        request_body["sort"] = should_sort_str
        message = {"request": request_body}
        request_json = json.dumps(message)
        SendMessage(client, request_json)

        while not client.hasAnyMessages():
            continue
        response_str = ReceiveMessage(client)
        response_json = json.loads(response_str, strict=False)
        VisualizeSlice(response_json)

        sub_window.destroy()

    sub_window = tk.Toplevel(root)
    sub_window.title("Get Slice")

    sort_var = tk.BooleanVar()
    sort_checkbox = tk.Checkbutton(sub_window, text="Need to sort", variable=sort_var)
    sort_checkbox.pack()

    listbox = tk.Listbox(sub_window)
    listbox.pack()

    add_pair_button = tk.Button(sub_window, text="Add Pair", command=add_pair)
    add_pair_button.pack()

    submit_button = tk.Button(sub_window, text="Submit", command=submit)
    submit_button.pack()

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
    print(cube)

    dates = []
    min_prices = []
    max_prices = []
    for entry in cube:
        data = entry["keys"][0]
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

def VisualizeRecordsPerData(response):
    cube = response["cube"]

    sub_window = tk.Toplevel(root)
    sub_window.title("Record per data")
    sub_window.geometry("800x600")

    listbox = tk.Listbox(sub_window)
    listbox.configure(width=800, height=600)
    listbox.pack()

    values = []
    for entry in cube:
        keys = entry["keys"]
        current_value = entry["value"]
        values.append(current_value)
        listbox.insert(tk.END, keys)

    def open_new_window(index):
        sub_window = tk.Toplevel(root)
        sub_window.title("Selected Item")
        sub_window.geometry("800x600")

        entry_value = json.loads(values[index])
        entry_list_box = tk.Listbox(sub_window)
        entry_list_box.configure(width=800, height=600)
        entry_list_box.pack()

        for value in entry_value:
            entry_list_box.insert(tk.END, json.dumps(value))

    def on_item_selected(event):
        selected_index = listbox.curselection()[0]
        open_new_window(selected_index)

    listbox.bind("<<ListboxSelect>>", on_item_selected)

def get_diagram_func():
    def open_sub_window():
        sub_window = tk.Toplevel(root)
        sub_window.title("Select diagram type")

        operation_var = tk.StringVar()
        operation_list = ["TOTAL_PRICE_PER_DATA", "PRODUCTS_COUNT", "DELIVERIES_PER_DATA_AND_TIME",
                          "DELIVERIES_PER_DISTANCE_AND_TIME", "PRICE_RANGE_PER_DATA", "COURIERS_REVENUE",
                          "RECORDS_PER_DATA"]

        operation_label = tk.Label(sub_window, text="Select operation:")
        operation_label.pack()

        operation_menu = tk.OptionMenu(sub_window, operation_var, *operation_list)
        operation_menu.pack()

        should_sort_var = tk.BooleanVar()
        should_sort_checkbox = tk.Checkbutton(sub_window, text="Should sort", variable=should_sort_var)
        should_sort_checkbox.pack()

        def call_function():
            diagram_type = operation_var.get()
            should_sort = should_sort_var.get()
            should_sort_str = "no"
            if should_sort:
                should_sort_str = "yes"

            request_body = {}
            request_body["request-type"] = "GET_DIAGRAM"
            request_body["diagram-type"] = diagram_type
            request_body["sort"] = should_sort_str
            message = {"request": request_body}
            request_json = json.dumps(message)
            SendMessage(client, request_json)

            while not client.hasAnyMessages():
                continue
            response_str = ReceiveMessage(client)
            response_json = json.loads(response_str, strict=False)

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
                VisualizeRecordsPerData(response_json)

            sub_window.destroy()

        submit_button = tk.Button(sub_window, text="Submit", command=call_function)
        submit_button.pack()

    sub_window = tk.Toplevel(root)
    sub_window.geometry("800x600")
    sub_window.title("Get Diagram")

    select_button = tk.Button(sub_window, text="Select diagram type", command=open_sub_window)
    select_button.pack()


if __name__ == '__main__':
    global client
    client = Client()
    client.run(PORT)

    root = tk.Tk()
    root.title("Main Menu")
    root.geometry("800x600")

    reload_button = tk.Button(root, text="RELOAD", command=reload_func)
    reload_button.pack()

    save_button = tk.Button(root, text="SAVE", command=save_func)
    save_button.pack()

    add_record_button = tk.Button(root, text="ADD_RECORD", command=add_record_func)
    add_record_button.pack()

    remove_record_button = tk.Button(root, text="REMOVE_RECORD", command=remove_record_func)
    remove_record_button.pack()

    get_records_button = tk.Button(root, text="GET_RECORDS", command=get_records_func)
    get_records_button.pack()

    get_slice_button = tk.Button(root, text="GET_SLICE", command=get_slice_func)
    get_slice_button.pack()

    get_diagram_button = tk.Button(root, text="GET_DIAGRAM", command=get_diagram_func)
    get_diagram_button.pack()

    root.mainloop()
