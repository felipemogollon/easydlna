package com.easydlna.dlna.service;

public interface IDMCCallback extends android.os.IInterface {
    /** Local-side IPC implementation stub class. */
    public static abstract class Stub extends android.os.Binder implements
            com.easydlna.dlna.service.IDMCCallback {
        private static final java.lang.String DESCRIPTOR = "com.easydlna.dlna.service.IDMCCallback";

        /** Construct the stub at attach it to the interface. */
        public Stub() {
            this.attachInterface(this, DESCRIPTOR);
        }

        /**
         * Cast an IBinder object into an com.easydlna.dlna.service.IDMCCallback
         * interface, generating a proxy if needed.
         */
        public static com.easydlna.dlna.service.IDMCCallback asInterface(
                android.os.IBinder obj) {
            if ((obj == null)) {
                return null;
            }
            android.os.IInterface iin = obj.queryLocalInterface(DESCRIPTOR);
            if (((iin != null) && (iin instanceof com.easydlna.dlna.service.IDMCCallback))) {
                return ((com.easydlna.dlna.service.IDMCCallback) iin);
            }
            return new com.easydlna.dlna.service.IDMCCallback.Stub.Proxy(obj);
        }

        @Override
        public android.os.IBinder asBinder() {
            return this;
        }

        @Override
        public boolean onTransact(int code, android.os.Parcel data,
                android.os.Parcel reply, int flags)
                throws android.os.RemoteException {
            switch (code) {
            case INTERFACE_TRANSACTION: {
                reply.writeString(DESCRIPTOR);
                return true;
            }
            case TRANSACTION_DeviceNotify: {
                data.enforceInterface(DESCRIPTOR);
                com.easydlna.dlna.service.DlnaDevice _arg0;
                if ((0 != data.readInt())) {
                    _arg0 = com.easydlna.dlna.service.DlnaDevice.CREATOR
                            .createFromParcel(data);
                } else {
                    _arg0 = null;
                }
                this.DeviceNotify(_arg0);
                reply.writeNoException();
                return true;
            }
            case TRANSACTION_ContentNotify: {
                data.enforceInterface(DESCRIPTOR);
                java.lang.String _arg0;
                _arg0 = data.readString();
                int _arg1;
                _arg1 = data.readInt();
                int _arg2;
                _arg2 = data.readInt();
                int _arg3;
                _arg3 = data.readInt();
                this.ContentNotify(_arg0, _arg1, _arg2, _arg3);
                reply.writeNoException();
                return true;
            }
            case TRANSACTION_PlayStatusNotify: {
                data.enforceInterface(DESCRIPTOR);
                java.lang.String _arg0;
                _arg0 = data.readString();
                com.easydlna.dlna.service.RenderStatus _arg1;
                if ((0 != data.readInt())) {
                    _arg1 = com.easydlna.dlna.service.RenderStatus.CREATOR
                            .createFromParcel(data);
                } else {
                    _arg1 = null;
                }
                this.PlayStatusNotify(_arg0, _arg1);
                reply.writeNoException();
                return true;
            }
            }
            return super.onTransact(code, data, reply, flags);
        }

        private static class Proxy implements
                com.easydlna.dlna.service.IDMCCallback {
            private android.os.IBinder mRemote;

            Proxy(android.os.IBinder remote) {
                mRemote = remote;
            }

            @Override
            public android.os.IBinder asBinder() {
                return mRemote;
            }

            public java.lang.String getInterfaceDescriptor() {
                return DESCRIPTOR;
            }

            @Override
            public void DeviceNotify(com.easydlna.dlna.service.DlnaDevice device)
                    throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    if ((device != null)) {
                        _data.writeInt(1);
                        device.writeToParcel(_data, 0);
                    } else {
                        _data.writeInt(0);
                    }
                    mRemote.transact(Stub.TRANSACTION_DeviceNotify, _data,
                            _reply, 0);
                    _reply.readException();
                } finally {
                    _reply.recycle();
                    _data.recycle();
                }
            }

            @Override
            public void ContentNotify(java.lang.String udn, int VideoCounts,
                    int AudioCounts, int ImageCounts)
                    throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    _data.writeString(udn);
                    _data.writeInt(VideoCounts);
                    _data.writeInt(AudioCounts);
                    _data.writeInt(ImageCounts);
                    mRemote.transact(Stub.TRANSACTION_ContentNotify, _data,
                            _reply, 0);
                    _reply.readException();
                } finally {
                    _reply.recycle();
                    _data.recycle();
                }
            }

            @Override
            public void PlayStatusNotify(java.lang.String udn,
                    com.easydlna.dlna.service.RenderStatus renderStatus)
                    throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    _data.writeString(udn);
                    if ((renderStatus != null)) {
                        _data.writeInt(1);
                        renderStatus.writeToParcel(_data, 0);
                    } else {
                        _data.writeInt(0);
                    }
                    mRemote.transact(Stub.TRANSACTION_PlayStatusNotify, _data,
                            _reply, 0);
                    _reply.readException();
                } finally {
                    _reply.recycle();
                    _data.recycle();
                }
            }
        }

        static final int TRANSACTION_DeviceNotify = (android.os.IBinder.FIRST_CALL_TRANSACTION + 0);
        static final int TRANSACTION_ContentNotify = (android.os.IBinder.FIRST_CALL_TRANSACTION + 1);
        static final int TRANSACTION_PlayStatusNotify = (android.os.IBinder.FIRST_CALL_TRANSACTION + 2);
    }

    public void DeviceNotify(com.easydlna.dlna.service.DlnaDevice device)
            throws android.os.RemoteException;

    public void ContentNotify(java.lang.String udn, int VideoCounts,
            int AudioCounts, int ImageCounts) throws android.os.RemoteException;

    public void PlayStatusNotify(java.lang.String udn,
            com.easydlna.dlna.service.RenderStatus renderStatus)
            throws android.os.RemoteException;
}
