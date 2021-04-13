from flask import Flask, jsonify
app = Flask(__name__)

response = {
			"id": 13,
			"name": "esp32_test_device",
			"macAddress": "30:ae:a4:27:49:6c",
			"token": "Bearer eyJhbGciOiJIUzUxMiJ9.eyJqdGkiOiJ5NHNlSldUIiwic3ViIjoiMzA6YWU6YTQ6Mjc6NDk6NmMiLCJhdXRob3JpdGllcyI6WyJST0xFX0RFVklDRSJdfQ.dHs7BmHtYmcb8DYJIItaSkcWgllJbL4x0v5npfF6wMAZjdL_rUkiy19eZJ9JqkYESjbKq-sdiiBqbmiHspMIkQ",
			"batteryLevel": 0,
			"lastSync": "2020-12-11T03:05:37.000+00:00",
			"latitude": 0.0,
			"longitude": 0.0,
			"otp": "38e0C3",
			"present": False
}

@app.route('/y4se/registerDevice', methods=['POST'])
def test():
	return jsonify(response)

if __name__ == '__main__':
	app.run(host = '0.0.0.0', port = 8088, debug = True)

