# Data transfer from pandas to DAPHNE and back, via files.  
import pandas as pd
from daphne.context.daphne_context import DaphneContext

ser1 = pd.Series([1e10, 2e10, 3e10])

dctx = DaphneContext()

dctx.from_pandas(ser1, shared_memory=False).print().compute(type="files")