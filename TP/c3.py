C3_OPERATIONS = ['m','M']


'''
devuelve el overhead agregado por C3
por ahora solo es el codigo del comando
'''
def c3_overhead():
    return 1


def c3_add_op(msg,op):
    return op + msg


def c3_process(msg,op):
    if(op=='m'):
        msg = msg.lower()
    if(op=='M'):
        msg = msg.upper()
    return msg

