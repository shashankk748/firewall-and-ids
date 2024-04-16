from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.chrome.service import Service
import time

from flask import Flask
 
# Flask constructor takes the name of 
# current module (__name__) as argument.
app = Flask(__name__)


service = Service(executable_path='C:\\chromedriver.exe')

#create chromeoptions instance
options = webdriver.ChromeOptions()
# options.add_argument("--headless")
# options.add_argument("--no-sandbox")
options.add_experimental_option("detach", True)
# #provide location where chrome stores profiles
options.add_argument("--user-data-dir=C:\\Users\\shash\\AppData\\Local\\Google\\Chrome\\User Data")

#provide the profile name with which we want to open browser
options.add_argument('--profile-directory=Default')

#specify where your chrome driver present in your pcs
driver = webdriver.Chrome(service=service, options=options)

@app.route('/')
def hello_world():
    driver.get("https://web.whatsapp.com/send?phone=+918744969975&text=Unauthorized%20Access%20Detected")
    time.sleep(15)
    selectable_texts = driver.find_elements(By.CSS_SELECTOR, "#main > footer > div._ak1k._ahmw.copyable-area > div > span:nth-child(2) > div > div._ak1r > div._ak1t._ak1u > button")
    print(selectable_texts)
    for box in selectable_texts:
        try:
            box.click()
        except: pass
    return 'Hello World'
 
# main driver function
if __name__ == '__main__':
    app.run(host="0.0.0.0")