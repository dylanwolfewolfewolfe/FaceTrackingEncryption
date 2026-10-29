
import cv2
import os
import ctypes
import numpy as np

vid = cv2.VideoCapture(0)

n = 8
Array = ctypes.c_int * n

class PublicKey(ctypes.Structure):
    _fields_ = [("a", Array), ("b", Array)]

class PrivateKey(ctypes.Structure):
    _fields_ = [("s", Array)]

class Ciphertext(ctypes.Structure):
    _fields_ = [("u", Array), ("v", Array)]

os.add_dll_directory(r"C:\msys64\ucrt64\bin")
crypto = ctypes.CDLL(os.path.join(os.path.dirname(__file__), "ring_lwe.dll"))

crypto.key_generation.argtypes = [ctypes.POINTER(PublicKey), ctypes.POINTER(PrivateKey)]
crypto.key_encryption.argtypes = [ctypes.POINTER(PublicKey), ctypes.POINTER(ctypes.c_int), ctypes.POINTER(Ciphertext)]
crypto.key_decryption.argtypes = [ctypes.POINTER(PrivateKey), ctypes.POINTER(Ciphertext), ctypes.POINTER(ctypes.c_int)]

def key_generation():
    pk = PublicKey()
    sk = PrivateKey()
    crypto.key_generation(ctypes.byref(pk), ctypes.byref(sk))
    return pk, sk

def key_encryption(byte):
    bits = Array(*[(byte >> i) & 1 for i in range(n)])
    ct = Ciphertext()
    crypto.key_encryption(ctypes.byref(pk), bits, ctypes.byref(ct))
    return ct

def key_decryption(ct):
    bits = Array()
    crypto.key_decryption(ctypes.byref(sk), ctypes.byref(ct), bits)
    return sum((bits[i] & 1) << i for i in range(n))

def process_encrypted_frame(frame):
    b = frame[:, :, 0]
    g = frame[:, :, 1]
    r = frame[:, :, 2]

    encrypted = []

    encrypted_channels = []

    for channel in (b, g, r):
        values = []

        for byte in channel.flatten():
            ct = key_encryption(int(byte))
            encrypted.append(ct)
            values.append(ct.v[0] % 256)

        encrypted_channels.append(
            np.array(values, dtype=np.uint8).reshape(channel.shape)
        )

    encrypted_frames = cv2.merge(tuple(encrypted_channels))
    return encrypted_frames, encrypted

def process_decrypted_frame(encrypted, shape):
    decrypted = []

    for ct in encrypted:
        decrypted.append(key_decryption(ct))

    size = shape[0] * shape[1]

    b = np.array(decrypted[:size], dtype=np.uint8).reshape(shape[:2])
    g = np.array(decrypted[size:2*size], dtype=np.uint8).reshape(shape[:2])
    r = np.array(decrypted[2*size:], dtype=np.uint8).reshape(shape[:2])

    decrypted_frames = cv2.merge((b, g, r))
    return decrypted_frames

def main():
    global pk, sk
    pk, sk = key_generation()

    cascade_path = cv2.data.haarcascades + "haarcascade_frontalface_default.xml"

    face_cascade = cv2.CascadeClassifier(cascade_path)

    try:
        while True:
            ret, frame = vid.read()

            if not ret:
                print("Failed to capture frame. Check webcam connection.")
                break

            gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
            faces = face_cascade.detectMultiScale(
                gray, scaleFactor=1.1, minNeighbors=5, minSize=(30, 30)
            )

            for (x, y, w, h) in faces:
                cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 255, 0), 2)

            cv2.imshow("original", frame)

            if len(faces) > 0:
                face_frame = frame[y:y+h, x:x+w]
                small_face = cv2.resize(face_frame, (32, 32))

                encrypted_frame, encrypted = process_encrypted_frame(small_face)

                combined_frame = frame.copy()
                combined_frame[y:y+h, x:x+w] = cv2.resize(
                    encrypted_frame, (w, h)
                )
                cv2.imshow("encrypted_frames", combined_frame)

                decrypted_frame = process_decrypted_frame(encrypted, small_face.shape)

                combined_decrypted_frame = frame.copy()
                combined_decrypted_frame[y:y+h, x:x+w] = cv2.resize(
                    decrypted_frame, (w, h)
                )
                cv2.imshow("decrypted_frame", combined_decrypted_frame)

            else:
                cv2.imshow("encrypted_frames", frame)
                cv2.imshow("decrypted_frame", frame)

            if cv2.waitKey(1) & 0xFF == ord('q'):
                break

    except Exception as e:
        print("Error occurred:", e)

    finally:
        vid.release()
        cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
