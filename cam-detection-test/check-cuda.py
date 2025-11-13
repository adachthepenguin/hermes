import torch, platform
print("Torch:", torch.__version__, "| Python:", platform.python_version(), "| OS:", platform.platform())
print("CUDA available:", torch.cuda.is_available())
print("MPS available:", getattr(torch.backends, "mps", None) and torch.backends.mps.is_available())
