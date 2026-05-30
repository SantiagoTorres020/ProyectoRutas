import pandas as pd

nodos = pd.read_csv("nodes.csv")
aristas = pd.read_csv("edges.csv")

print("Nodos originales:", len(nodos))
print("Aristas originales:", len(aristas))

nodos["node_id"] = pd.to_numeric(nodos["node_id"], errors="coerce")
nodos["lat"] = pd.to_numeric(nodos["lat"], errors="coerce")
nodos["lon"] = pd.to_numeric(nodos["lon"], errors="coerce")

nodos = nodos.dropna(subset=["node_id", "lat", "lon"])

nodos = nodos.drop_duplicates(subset=["node_id"])

nodos["node_id"] = nodos["node_id"].astype(int)

aristas["from_id"] = pd.to_numeric(aristas["from_id"], errors="coerce")
aristas["to_id"] = pd.to_numeric(aristas["to_id"], errors="coerce")
aristas["distance_m"] = pd.to_numeric(aristas["distance_m"], errors="coerce")

aristas = aristas.dropna(subset=["from_id", "to_id", "distance_m"])

aristas["from_id"] = aristas["from_id"].astype(int)
aristas["to_id"] = aristas["to_id"].astype(int)

aristas = aristas[aristas["distance_m"] > 0]

aristas = aristas[aristas["from_id"] != aristas["to_id"]]

aristas["fclass"] = aristas["fclass"].fillna("unknown")
aristas["fclass"] = aristas["fclass"].astype(str).str.strip()
aristas.loc[aristas["fclass"] == "", "fclass"] = "unknown"

aristas["maxspeed"] = (
    aristas["maxspeed"]
    .astype(str)
    .str.extract(r"(\d+)", expand=False)
)

aristas["maxspeed"] = (
    pd.to_numeric(aristas["maxspeed"], errors="coerce")
    .fillna(0)
    .astype(int)
)

aristas["oneway"] = aristas["oneway"].fillna(0)
aristas["oneway"] = aristas["oneway"].astype(str).str.strip().str.lower()

aristas["oneway"] = aristas["oneway"].replace({
    "yes": "1",
    "true": "1",
    "t": "1",
    "si": "1",
    "no": "0",
    "false": "0",
    "f": "0"
})

aristas["oneway"] = (
    pd.to_numeric(aristas["oneway"], errors="coerce")
    .fillna(0)
    .astype(int)
)

aristas.loc[~aristas["oneway"].isin([0, 1]), "oneway"] = 0

ids_nodos = set(nodos["node_id"])

aristas = aristas[
    aristas["from_id"].isin(ids_nodos) &
    aristas["to_id"].isin(ids_nodos)
]

aristas = aristas.drop_duplicates(
    subset=[
        "from_id",
        "to_id",
        "distance_m",
        "fclass",
        "oneway",
        "maxspeed"
    ]
)

nodos.to_csv("nodes_limpio.csv", index=False)
aristas.to_csv("edges_limpio.csv", index=False)

print("Nodos limpios:", len(nodos))
print("Aristas limpias:", len(aristas))
print("Archivos generados: nodes_limpio.csv y edges_limpio.csv")