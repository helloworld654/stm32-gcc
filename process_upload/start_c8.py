import os
import time
import logging

logger = ''

def log_init():
  global logger
  # logging.basicConfig(filename='logs.log', filemode='w',format='%(levelname)s -> %(asctime)s: %(message)s', level=logging.DEBUG)
  logging.basicConfig(filename='logs.log', filemode='a',format='%(levelname)s -> %(asctime)s: %(message)s', level=logging.INFO)
  logger = logging.getLogger(__name__)
  save_log('init log success')

def save_log(str):
  logger.info(str)

if __name__ == '__main__':
  #log_init()
  print('start process c8 project')
  #save_log('start process c8')
  os.system('./bef_make.sh')
  os.system('./exchange_code.sh')
  os.system('./c8make.sh')
  os.system('./aft_make.sh')
  print("finished of c8 project")
  #save_log('finished of c8 process')

