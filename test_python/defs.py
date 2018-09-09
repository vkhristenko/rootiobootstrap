import ctypes

def wrap_cfunc(lib, fname, result, *arg_types):
    return ctypes.CFUNCTYPE(result, *arg_types)((fname, lib))

class Structure(ctypes.Structure):
    def __repr__(self):
        """print the fields"""
        res = []
        print self.__class__.__name__
        for field in self._fields_: 
            print field[1]
            res.append("%s=%s" % (field[0], repr(getattr(self, field[0]))))
        print self.__class__.__name__
        return self.__class__.__name__ + "(" + ",".join(res) + ")"

    # TODO: understand this
    @classmethod
    def from_param(cls, obj):
        """Magically construct from a tuple"""
        if isinstance(obj, cls):
            return obj
        if isinstance(obj, tuple):
            return cls(obj)
            
        raise TypeError
