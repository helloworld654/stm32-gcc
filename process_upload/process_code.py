import os
import time

if __name__ == '__main__':
  list_res_before_setup = list('\n')
  list_res_setup = list('\n')
  list_res_loop = list('\n')
  code_resA = '\n'
  code_resB = '\n'
  code_res_bef_setup = '\n'
  print('start to process code')
  time.sleep(0.02)
  f = open('/home/user000/upload/code.txt')
  full_str = f.read()
  f.close()
  full_list = full_str.split('\n')
  setupA = setupB = loopA = loopB = -1
  for i in range(len(full_list)):
    if('volatile' in full_list[i]):
      full_list.remove(full_list[i])
    if('setup()' in full_list[i]):
      setupA = i
      continue
    if('loop()' in full_list[i]):
      loopA = i
      continue
    if(setupA!=-1 and loopA!=-1):
      break
  setupB = loopA -2;
  loopB = len(full_list) - 1
  # print("setupA:%d,setupB:%d,loopA:%d,loopB:%d"%(setupA,setupB,loopA,loopB))
  if setupA != 0:
    for i in range(setupA):
      list_res_before_setup.insert(len(list_res_before_setup),full_list[i])
    code_res_bef_setup = ''.join(list_res_before_setup)
  for i in range(setupA+1,setupB):
    list_res_setup.insert(len(list_res_setup),full_list[i])
    list_res_setup.insert(len(list_res_setup),'\n')
  code_resA = ''.join(list_res_setup)
  for i in range(loopA+1,loopB):
    list_res_loop.insert(len(list_res_loop),full_list[i])
    list_res_loop.insert(len(list_res_loop),'\n')
  code_resB = ''.join(list_res_loop)

  #print(code_resA)
  #print('-------------')
  #print(code_resB)
  
  f = open('../Src/main.c','r')
  main_str = f.read()
  f.close()
  str_list = list(main_str)

  find = main_str.find('init functions')
  if find != -1:
    str_list.insert(find+14,code_resA)
    main_str = ''.join(str_list)
    str_list = list(main_str)
  else:
    print('can not find the str:init functions')

  find = main_str.find('while functions')
  if find != -1:
    str_list.insert(find+15,code_resB)
    main_str = ''.join(str_list)
    str_list = list(main_str)
  else:
    print('can not find the str:while functions')

  find = main_str.find('pre-def functions')
  if find != -1:
    str_list.insert(find+17,code_res_bef_setup)
    main_str = ''.join(str_list)
  else:
    print('can not find the str:while functions')

  f = open('../Src/main.c','w')
  f.write(main_str)
  f.close()
  print('end of process code')
