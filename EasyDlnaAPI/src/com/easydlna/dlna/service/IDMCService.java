package com.easydlna.dlna.service;

public interface IDMCService extends android.os.IInterface {
    /** Local-side IPC implementation stub class. */
    public static abstract class Stub extends android.os.Binder implements
            com.easydlna.dlna.service.IDMCService {
        private static final java.lang.String DESCRIPTOR = "com.easydlna.dlna.service.IDMCService";

        /** Construct the stub at attach it to the interface. */
        public Stub() {
            this.attachInterface(this, DESCRIPTOR);
        }

        /**
         * Cast an IBinder object into an com.easydlna.dlna.service.IDMCService
         * interface, generating a proxy if needed.
         */
        public static com.easydlna.dlna.service.IDMCService asInterface(
                android.os.IBinder obj) {
            if ((obj == null)) {
                return null;
            }
            android.os.IInterface iin = obj.queryLocalInterface(DESCRIPTOR);
            if (((iin != null) && (iin instanceof com.easydlna.dlna.service.IDMCService))) {
                return ((com.easydlna.dlna.service.IDMCService) iin);
            }
            return new com.easydlna.dlna.service.IDMCService.Stub.Proxy(obj);
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
            case TRANSACTION_start_stop_DMC: {
                data.enforceInterface(DESCRIPTOR);
                boolean _arg0;
                _arg0 = (0 != data.readInt());
                this.start_stop_DMC(_arg0);
                reply.writeNoException();
                return true;
            }
            case TRANSACTION_addDMCCallback: {
                data.enforceInterface(DESCRIPTOR);
                com.easydlna.dlna.service.IDMCCallback _arg0;
                _arg0 = com.easydlna.dlna.service.IDMCCallback.Stub
                        .asInterface(data.readStrongBinder());
                this.addDMCCallback(_arg0);
                reply.writeNoException();
                return true;
            }
            case TRANSACTION_removeDMCCallback: {
                data.enforceInterface(DESCRIPTOR);
                com.easydlna.dlna.service.IDMCCallback _arg0;
                _arg0 = com.easydlna.dlna.service.IDMCCallback.Stub
                        .asInterface(data.readStrongBinder());
                this.removeDMCCallback(_arg0);
                reply.writeNoException();
                return true;
            }
            case TRANSACTION_getOnLineDevices: {
                data.enforceInterface(DESCRIPTOR);
                java.util.List<com.easydlna.dlna.service.DlnaDevice> _arg0;
                _arg0 = new java.util.ArrayList<com.easydlna.dlna.service.DlnaDevice>();
                int _arg1;
                _arg1 = data.readInt();
                this.getOnLineDevices(_arg0, _arg1);
                reply.writeNoException();
                reply.writeTypedList(_arg0);
                return true;
            }
            case TRANSACTION_searchDevice: {
                data.enforceInterface(DESCRIPTOR);
                int _arg0;
                _arg0 = data.readInt();
                this.searchDevice(_arg0);
                reply.writeNoException();
                return true;
            }
            case TRANSACTION_playControl: {
                data.enforceInterface(DESCRIPTOR);
                java.lang.String _arg0;
                _arg0 = data.readString();
                int _arg1;
                _arg1 = data.readInt();
                java.lang.String _arg2;
                _arg2 = data.readString();
                int _result = this.playControl(_arg0, _arg1, _arg2);
                reply.writeNoException();
                reply.writeInt(_result);
                return true;
            }
            case TRANSACTION_getContents: {
                data.enforceInterface(DESCRIPTOR);
                java.lang.String _arg0;
                _arg0 = data.readString();
                int _arg1;
                _arg1 = data.readInt();
                java.util.List<com.easydlna.dlna.service.ContentInfo> _arg2;
                _arg2 = new java.util.ArrayList<com.easydlna.dlna.service.ContentInfo>();
                int _arg3;
                _arg3 = data.readInt();
                int _arg4;
                _arg4 = data.readInt();
                int _result = this.getContents(_arg0, _arg1, _arg2, _arg3,
                        _arg4);
                reply.writeNoException();
                reply.writeInt(_result);
                reply.writeTypedList(_arg2);
                return true;
            }
            case TRANSACTION_refreshContents: {
                data.enforceInterface(DESCRIPTOR);
                java.lang.String _arg0;
                _arg0 = data.readString();
                int _result = this.refreshContents(_arg0);
                reply.writeNoException();
                reply.writeInt(_result);
                return true;
            }
            case TRANSACTION_setAutoStartup: {
                data.enforceInterface(DESCRIPTOR);
                boolean _arg0;
                _arg0 = (0 != data.readInt());
                boolean _result = this.setAutoStartup(_arg0);
                reply.writeNoException();
                reply.writeInt(((_result) ? (1) : (0)));
                return true;
            }
            }
            return super.onTransact(code, data, reply, flags);
        }

        private static class Proxy implements
                com.easydlna.dlna.service.IDMCService {
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
            public void start_stop_DMC(boolean bStart)
                    throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    _data.writeInt(((bStart) ? (1) : (0)));
                    mRemote.transact(Stub.TRANSACTION_start_stop_DMC, _data,
                            _reply, 0);
                    _reply.readException();
                } finally {
                    _reply.recycle();
                    _data.recycle();
                }
            }

            @Override
            public void addDMCCallback(
                    com.easydlna.dlna.service.IDMCCallback callback)
                    throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    _data.writeStrongBinder((((callback != null)) ? (callback
                            .asBinder()) : (null)));
                    mRemote.transact(Stub.TRANSACTION_addDMCCallback, _data,
                            _reply, 0);
                    _reply.readException();
                } finally {
                    _reply.recycle();
                    _data.recycle();
                }
            }

            @Override
            public void removeDMCCallback(
                    com.easydlna.dlna.service.IDMCCallback callback)
                    throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    _data.writeStrongBinder((((callback != null)) ? (callback
                            .asBinder()) : (null)));
                    mRemote.transact(Stub.TRANSACTION_removeDMCCallback, _data,
                            _reply, 0);
                    _reply.readException();
                } finally {
                    _reply.recycle();
                    _data.recycle();
                }
            }

            @Override
            public void getOnLineDevices(
                    java.util.List<com.easydlna.dlna.service.DlnaDevice> onLineDevicesList,
                    int type) throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    _data.writeInt(type);
                    mRemote.transact(Stub.TRANSACTION_getOnLineDevices, _data,
                            _reply, 0);
                    _reply.readException();
                    _reply.readTypedList(onLineDevicesList,
                            com.easydlna.dlna.service.DlnaDevice.CREATOR);
                } finally {
                    _reply.recycle();
                    _data.recycle();
                }
            }

            @Override
            public void searchDevice(int type)
                    throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    _data.writeInt(type);
                    mRemote.transact(Stub.TRANSACTION_searchDevice, _data,
                            _reply, 0);
                    _reply.readException();
                } finally {
                    _reply.recycle();
                    _data.recycle();
                }
            }

            @Override
            public int playControl(java.lang.String deviceUDN,
                    int CONTORL_TYPE, java.lang.String param)
                    throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                int _result;
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    _data.writeString(deviceUDN);
                    _data.writeInt(CONTORL_TYPE);
                    _data.writeString(param);
                    mRemote.transact(Stub.TRANSACTION_playControl, _data,
                            _reply, 0);
                    _reply.readException();
                    _result = _reply.readInt();
                } finally {
                    _reply.recycle();
                    _data.recycle();
                }
                return _result;
            }

            @Override
            public int getContents(
                    java.lang.String deviceUDN,
                    int type,
                    java.util.List<com.easydlna.dlna.service.ContentInfo> filesList,
                    int StartingIndex, int Count)
                    throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                int _result;
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    _data.writeString(deviceUDN);
                    _data.writeInt(type);
                    _data.writeInt(StartingIndex);
                    _data.writeInt(Count);
                    mRemote.transact(Stub.TRANSACTION_getContents, _data,
                            _reply, 0);
                    _reply.readException();
                    _result = _reply.readInt();
                    _reply.readTypedList(filesList,
                            com.easydlna.dlna.service.ContentInfo.CREATOR);
                } finally {
                    _reply.recycle();
                    _data.recycle();
                }
                return _result;
            }

            @Override
            public int refreshContents(java.lang.String deviceUDN)
                    throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                int _result;
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    _data.writeString(deviceUDN);
                    mRemote.transact(Stub.TRANSACTION_refreshContents, _data,
                            _reply, 0);
                    _reply.readException();
                    _result = _reply.readInt();
                } finally {
                    _reply.recycle();
                    _data.recycle();
                }
                return _result;
            }

            @Override
            public boolean setAutoStartup(boolean bAutoStartup)
                    throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                boolean _result;
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    _data.writeInt(((bAutoStartup) ? (1) : (0)));
                    mRemote.transact(Stub.TRANSACTION_setAutoStartup, _data,
                            _reply, 0);
                    _reply.readException();
                    _result = (0 != _reply.readInt());
                } finally {
                    _reply.recycle();
                    _data.recycle();
                }
                return _result;
            }
        }

        static final int TRANSACTION_start_stop_DMC = (android.os.IBinder.FIRST_CALL_TRANSACTION + 0);
        static final int TRANSACTION_addDMCCallback = (android.os.IBinder.FIRST_CALL_TRANSACTION + 1);
        static final int TRANSACTION_removeDMCCallback = (android.os.IBinder.FIRST_CALL_TRANSACTION + 2);
        static final int TRANSACTION_getOnLineDevices = (android.os.IBinder.FIRST_CALL_TRANSACTION + 3);
        static final int TRANSACTION_searchDevice = (android.os.IBinder.FIRST_CALL_TRANSACTION + 4);
        static final int TRANSACTION_playControl = (android.os.IBinder.FIRST_CALL_TRANSACTION + 5);
        static final int TRANSACTION_getContents = (android.os.IBinder.FIRST_CALL_TRANSACTION + 6);
        static final int TRANSACTION_refreshContents = (android.os.IBinder.FIRST_CALL_TRANSACTION + 7);
        static final int TRANSACTION_setAutoStartup = (android.os.IBinder.FIRST_CALL_TRANSACTION + 8);
    }

    public void start_stop_DMC(boolean bStart)
            throws android.os.RemoteException;

    public void addDMCCallback(com.easydlna.dlna.service.IDMCCallback callback)
            throws android.os.RemoteException;

    public void removeDMCCallback(
            com.easydlna.dlna.service.IDMCCallback callback)
            throws android.os.RemoteException;

    public void getOnLineDevices(
            java.util.List<com.easydlna.dlna.service.DlnaDevice> onLineDevicesList,
            int type) throws android.os.RemoteException;

    public void searchDevice(int type) throws android.os.RemoteException;

    public int playControl(java.lang.String deviceUDN, int CONTORL_TYPE,
            java.lang.String param) throws android.os.RemoteException;

    public int getContents(java.lang.String deviceUDN, int type,
            java.util.List<com.easydlna.dlna.service.ContentInfo> filesList,
            int StartingIndex, int Count) throws android.os.RemoteException;

    public int refreshContents(java.lang.String deviceUDN)
            throws android.os.RemoteException;

    public boolean setAutoStartup(boolean bAutoStartup)
            throws android.os.RemoteException;
}
