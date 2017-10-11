import cos_module
import numpy as np
import pylab

print cos_module.cos_func(0.0)
print cos_module.cos_func(1.0)
print cos_module.cos_func(np.pi / 2)
print cos_module.cos_func(np.pi)

to_cos = {
    "zero": 0.0,
    "one": 1.0,
    "half_pi": np.pi / 2,
    "pi": np.pi
}

print cos_module.cos_func_dict(to_cos)
x = np.arange(0, 2 * np.pi, 0.1)
y = cos_module.cos_func_np(x)
pylab.plot(x, y)
pylab.show()
