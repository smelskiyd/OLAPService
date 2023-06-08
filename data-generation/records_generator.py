import random
import datetime

dates = ["2023.{:02d}.05".format(day) for day in range(1, 32)]
times = ["{:02d}:00".format(hour) for hour in range(24)]
from_places = ["A", "B", "C", "D", "E", "F"]
to_places = ["A", "B", "C", "D", "E", "F"]
product_count = 30
products = ["Product{}".format(i) for i in range(1, product_count + 1)]
couriers = [i for i in range(1, 20)]
distances = [random.randint(1, 200) for _ in range(500)]
prices = [random.randint(1, 100) for _ in range(500)]

records = []
for _ in range(500):
    date = random.choice(dates)
    hour = int(random.gauss(12, 4))
    hour = max(0, min(23, hour))
    time = datetime.time(hour=hour, minute=0)
    from_place = random.choice(from_places)
    to_place = random.choice(to_places)
    product = random.choice(products)
    courier = random.choice(couriers)
    distance = int(random.expovariate(1 / 100))
    price = int(random.expovariate(1 / 50))

    record = f"{date} {time.strftime('%H:%M')} {from_place} {to_place} {product} {courier} {distance} {price}"
    records.append(record)

for record in records:
    print(record)
