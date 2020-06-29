import instance

def get_fields() -> list:
    '''
    Get instance fields (strings)
    '''
    import inspect
    members = list(map(lambda t: t[0],
        inspect.getmembers(instance.Instance,
            lambda a: not(inspect.isroutine(a)))))
    return [x for x in members if x[:2] != '__' and x[:-2] != '__']

# <python> <script> select <column1> <column2> <...>
if __name__ == '__main__':
    import sys
    data = instance.parse()
    fields = get_fields()
    args = [x.lower() for x in sys.argv[1:]]
    if args[0] == 'select':
        attrs = args[1:]
        header = False
        for i in data:
            iattrs = [getattr(i, a) for a in attrs]
            for idx, iattr in enumerate(iattrs):
                if type(iattr) is int:
                    iattrs[idx] = str(iattr)
                elif type(iattr) is not str:
                    iattrs[idx] = iattr.name
            width = 80
            nattrs = 1 + len(attrs)
            fstr = "{:" + str(80 // nattrs) + "}"
            if not header:
                header = True
                print((fstr * nattrs).format('name', *attrs))
                print("-"*width)
            print((fstr * nattrs).format(i.name, *iattrs))
