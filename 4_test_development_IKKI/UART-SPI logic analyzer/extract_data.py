import pandas as pd

def limpiar_paquetes(paquete):
    # Eliminar START y STOP y espacios extra
    return paquete.replace('START', '').replace('STOP', '').strip()

def agrupar_paquetes(lista_paquetes):
    # Agrupa cada dos elementos y los concatena
    paquetes_agrupados = []
    for i in range(0, len(lista_paquetes), 2):
        parte1 = lista_paquetes[i] if i < len(lista_paquetes) else ''
        parte2 = lista_paquetes[i+1] if i+1 < len(lista_paquetes) else ''
        paquetes_agrupados.append(parte1 + parte2)
    return paquetes_agrupados

# Cargar CSV (ajusta el nombre de archivo y delimitador si es necesario)
df = pd.read_csv('test2raw.csv')

# Asumimos que la segunda columna es de índice 1
# Limpia START y STOP
df['limpio'] = df.iloc[:,1].apply(limpiar_paquetes)

# Elimina posibles filas vacías después de limpiar
df = df[df['limpio'] != '']

# Obtén la lista de paquetes limpios
paquetes_limpios = df['limpio'].tolist()

# Agrupa los paquetes
paquetes_agrupados = agrupar_paquetes(paquetes_limpios)

# Crea un nuevo DataFrame
df_resultado = pd.DataFrame({'paquete_agrupado': paquetes_agrupados})

# Guarda el resultado si quieres
df_resultado.to_csv('paquetes_agrupados.csv', index=False)

print(df_resultado)
